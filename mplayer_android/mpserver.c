#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stropts.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "domain_socket.h"

#include "pthread-macro.h"


//#define NEW_MPSERVER_CMD 1
//#define MPSERVER_BENCH_DECODE_TIME 1
#if MPSERVER_BENCH_DECODE_TIME
#include "skydbg.c"
#endif


#ifdef USE_INSTRUMENT_FUNCTIONS
//===================
struct layout {
    struct layout *next;        /*! Holds the previous value of EBP (Frame Pointer)*/
    void *return_address;       /*! Holds the previous value of EIP (Return Address)*/
};


#define START_ADDR 0xb000
#define END_ADDR 0xb0000
unsigned int g_mpserver_func_parent_list[END_ADDR - START_ADDR];
unsigned int g_mpserver_func_child_list[END_ADDR - START_ADDR];
FILE *fp;

#define ADVANCE_STACK_FRAME(next) (struct layout *) ( next )
#define FIRST_FRAME_POINTER  __builtin_frame_address (0)
#define CURRENT_STACK_FRAME  ({ char __csf; &__csf; })

void __cyg_profile_func_enter( void *func_address, void *call_site )
__attribute__ ((no_instrument_function));
void __cyg_profile_func_exit ( void *func_address, void *call_site )
__attribute__ ((no_instrument_function));
void __cyg_profile_func_enter( void *this, void *callsite )
{
    /* Function Entry Address */
    {
        fprintf(fp, "E%p\n", (int *)this);
//	  fprintf(stderr, "E%p  Call=%p\n", (int *)this, (int *)callsite);
        g_mpserver_func_parent_list[(unsigned int)callsite - START_ADDR]++;
        g_mpserver_func_child_list[(unsigned int)this - START_ADDR]++;
    }
}
void __cyg_profile_func_exit( void *this, void *callsite )
{
    fprintf(fp, "X%p\n", (int *)this);
#if 0
    /* Function Exit Address */
    fprintf(stderr, "X%p  Call=%p\n", (int *)this, (int *)callsite);
#endif
}

/* Constructor and Destructor Prototypes */

void main_constructor( void )
__attribute__ ((no_instrument_function, constructor));

void main_destructor( void )
__attribute__ ((no_instrument_function, destructor));

/* Output trace file pointer */
void main_constructor( void )
{
    fp = fopen( "/data/tmpfsmisc/mpserver.trace", "w" );
    if (fp == NULL) exit(-1);
}

void main_destructor( void )
{
    fclose(fp);
}
//===================
#endif

int mpserver_need_redirect_stdout = 0;
int code_domain = 0;
int skyfd = -1;

int mpserver_vdec_ridx=0, mpserver_vdec_widx=0;
int mpserver_vdec_cnts=0;
unsigned int mpserver_vdec_rcnts=0, mpserver_vdec_wcnts=0;
int mplayer_pid = 0;
int mpserver_pipefd_stdout[2];
int mpserver_pipefd_stderr[2];
//20110106 Robert add mpserver kill flag
int mpserver_kill_flag = 0;	//1: kill by command 4, 0: kill by mplayer itself

#define MAX_CMD_IDX 20
char *NewExecv[MAX_CMD_IDX];
void ipc_callback(char *datastr);
void ipc_callback_error(int status_id);



static void exit_sighandler(int x)
{
    if (x != 1)
        printf("\nMPSERVER: ====== got signal %d =====\n", x);
    if (x == 11 /*|| x == 13 || x == 6*/)
    {
        exit(0);
    }
    if (x== SIGCHLD)
    {
        int status;
        printf("MPSERVER: got signal SIGCHLD, try to waitpid %d\n", mplayer_pid);
        //waitpid(mplayer_pid, &status, WNOHANG);
	waitpid(mplayer_pid, &status, 0);
        mplayer_pid = 0;
	if (mpserver_kill_flag == 1)
	{
		//killed by command 4
		ipc_callback_error(2);
	}
	else
	{
		//mplayer exit by itself
		ipc_callback_error(1);
	}
    }
#ifdef USE_INSTRUMENT_FUNCTIONS
    if (x==3)
    {
        int i;
        printf("\n========================================\n");
        for (i=0; i<END_ADDR - START_ADDR; i+=4)
        {
            if (g_mpserver_func_parent_list[i] != 0 || g_mpserver_func_child_list[i] != 0)
            {
                printf("P Addr: 0x%.8x (%d)   C Addr: 0x%.8x (%d)\n", i+0xb000, g_mpserver_func_parent_list[i], i+0xb000, g_mpserver_func_child_list[i]);
            }

            g_mpserver_func_parent_list[i] = 0;
            g_mpserver_func_child_list[i] = 0;
        }
    }
#endif
}

void reset_pipe(void)
{
	if (mpserver_pipefd_stdout[0] != -1)
	{
		close(mpserver_pipefd_stdout[0]);
		mpserver_pipefd_stdout[0] = -1;
	}
	if (mpserver_pipefd_stdout[1] != -1)
	{
		close(mpserver_pipefd_stdout[1]);
		mpserver_pipefd_stdout[1] = -1;
	}
	if (mpserver_pipefd_stderr[0] != -1)
	{
		close(mpserver_pipefd_stderr[0]);
		mpserver_pipefd_stderr[0] = -1;
	}
	if (mpserver_pipefd_stderr[1] != -1)
	{
		close(mpserver_pipefd_stderr[1]);
		mpserver_pipefd_stderr[1] = -1;
	}
}

//send message to domain socket : mplayersock_callback
void ipc_callback(char *datastr)
{
    int     sockfd;
    struct	sockaddr_un serv_addr;
    int     result;
    void    *send_ptr;
    int		server_len;

    printf("MPSERVER: ipc_callback(%s)\n",datastr);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    server_len = sizeof(serv_addr.sun_family) +
                 sprintf(serv_addr.sun_path, " mplayersock_callback");

    serv_addr.sun_path[0] = 0;   //Use abstract name as domain socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("MPSERVER: %s - Cannot create socket: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }

    result = connect(sockfd, (struct sockaddr *)&serv_addr, server_len);
    if (result == -1)
    {
        printf("MPSERVER: %s - Cannot connect socket: %s\n",__FUNCTION__ , strerror(errno));
        close(sockfd);
        return;
    }

    send_ptr = datastr;
    result = send(sockfd,send_ptr, strlen(datastr), 0);
    if (result == -1)
        printf("MPSERVER: %d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    close(sockfd);
}

void ipc_callback_error(int status_id)
{
    char callback_str[24];

    sprintf(callback_str,"status: error: %d\0", status_id);
    ipc_callback(callback_str);
}

void mpserver_force_kill_child(void)
{
    int status, i, ret=-1;

//20110106 Robert add mpserver kill flag
    mpserver_kill_flag = 1;
    if (mplayer_pid != 0)
    {
	if (mplayer_pid != 0)
	        kill(mplayer_pid, 3);
        usleep(1000*30);
	if (mplayer_pid != 0)
	        kill(mplayer_pid, 3);
        usleep(1000*30);
	if (mplayer_pid != 0)
	        kill(mplayer_pid, 9);
        usleep(1000*30);
#if 0
        if (mplayer_pid)
	{
	    for (i=0; i<100; i++)
	    {
		    if (mplayer_pid != 0)
		            ret = waitpid(mplayer_pid, &status, WNOHANG);
		    else
			    break;
		    if (ret == 0)
			usleep(1000*10);
		    else if (ret == mplayer_pid)
			break;
	    }
//	    ipc_callback_error(2);
	}
#else
	for(i=0; i<100; i++){
		if(mplayer_pid == 0){
			printf("MPSERVER: wait mplayer exit done.\n");
			break;
		}
		usleep(1000*10);
	}

	if(i >= 100) printf("MPSERVER: wait mplayer(%d) exit timeout\n", mplayer_pid);
#endif
    }
}

static inline void mpserver_cnts_inc(void)
{
    mpserver_vdec_wcnts++;
}
static inline void mpserver_cnts_dec(void)
{
    mpserver_vdec_rcnts++;
}

void mpserver_usec_sleep(unsigned int usec_delay)
{
    struct timespec ts;
//    unsigned int usec_delay = 1 * 1000;
    ts.tv_sec  =  usec_delay / 1000000;
    ts.tv_nsec = (usec_delay % 1000000) * 1000;
    nanosleep(&ts, NULL);
}


void mpserver_video_pthread(void)
{
#if MPSERVER_BENCH_DECODE_TIME
    struct timeval tv_dec_start;
    gettimeofday(&sky_tv_start, 0);
#endif
    while (1)
    {

#if MPSERVER_BENCH_DECODE_TIME
        gettimeofday(&tv_dec_start, 0);
#endif

///
        if (1)
        {

#if MPSERVER_BENCH_DECODE_TIME
            struct timeval tv;
            int diff_lastdec_sec, diff_lastdec_usec;
            int diff_dec_sec, diff_dec_usec;
            gettimeofday(&tv, 0);
            diff_lastdec_sec = tv.tv_sec - sky_tv_start.tv_sec;
            if (tv.tv_usec >= sky_tv_start.tv_usec)
            {
                diff_lastdec_usec = tv.tv_usec - sky_tv_start.tv_usec;
            }
            else
            {
                diff_lastdec_usec = 1000000 - sky_tv_start.tv_usec + tv.tv_usec;
                diff_lastdec_sec--;
            }

            diff_dec_sec = tv.tv_sec - tv_dec_start.tv_sec;
            if (tv.tv_usec >= tv_dec_start.tv_usec)
            {
                diff_dec_usec = tv.tv_usec - tv_dec_start.tv_usec;
            }
            else
            {
                diff_dec_usec = 1000000 - tv_dec_start.tv_usec + tv.tv_usec;
                diff_dec_sec--;
            }
            if (diff_dec_usec > 33000)
                fprintf(stderr, "#### time -- last %d.%.6d,   cur dec %d.%.6d  [0]:%d wc:%d rc:%d\n", diff_lastdec_sec, diff_lastdec_usec, diff_dec_sec, diff_dec_usec, mpserver_shm_array[0], mpserver_vdec_wcnts, mpserver_vdec_rcnts);

            gettimeofday(&sky_tv_start, 0);
#endif

            mpserver_cnts_dec();
        }
        else
        {
#if 0
            pwait_event(&mpserver_cond, &mpserver_mtx);
#else
//            usleep(1);
            struct timespec ts;
            unsigned int usec_delay = 1 * 1000;
            ts.tv_sec  =  usec_delay / 1000000;
            ts.tv_nsec = (usec_delay % 1000000) * 1000;
            nanosleep(&ts, NULL);
#endif
//printf("MPSERVER: af usleep.. wcnt:%d rcnt:%d\n", mpserver_vdec_wcnts, mpserver_vdec_rcnts);
        }

    }
}


void mpserver_do_init_vqueue(void)
{
    mpserver_vdec_ridx=0;
    mpserver_vdec_widx=0;
    mpserver_vdec_cnts=0;
    mpserver_vdec_rcnts=0;
    mpserver_vdec_wcnts=0;
}

void ipc_handler()
{
    int		server_sockfd, client_sockfd = -1;
    int		server_len, client_len;
    struct	sockaddr_un serv_address;
    struct	sockaddr_un client_address;
    unsigned char   sdata[2048];
    GeneralPkt	send_pkt;
    int 	ret;
    int reuse=1;

    int mpserver_need_reset = 0;

    /*  Remove any old socket and create an unnamed socket for the server.  */
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
printf("\nMPSERVER: create new socket, server_sockfd=%d\n", server_sockfd);
    if (server_sockfd == -1)
    {
        printf("MPSERVER: %s - Cannot create socket: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }

    /*  Name the socket.  */
    serv_address.sun_family = AF_UNIX;
    server_len = sizeof(serv_address.sun_family) +
                 sprintf(serv_address.sun_path, " mpserversock");
    serv_address.sun_path[0] = 0;   //Use abstract name as domain socket

    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

#ifdef NEW_MPSERVER_CMD
    int sbuf, rbuf, len;
    len = sizeof(sbuf);
    getsockopt(server_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, &len);
    len = sizeof(rbuf);
    getsockopt(server_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
    printf("MPSERVER: send/receive socket buffer space %d/%d bytes\n", sbuf, rbuf);
#endif

    ret = bind(server_sockfd, (struct sockaddr *)&serv_address, server_len);
    if (ret == -1)
    {
        printf("MPSERVER: %s - Cannot bind: %s\n",__FUNCTION__ , strerror(errno));
        return;
    }


    /*  Create a connection queue and wait for clients.  */
    listen(server_sockfd, 5);
    while (1) {
        fd_set FD_rcv;
        int select_ret=0;
        int fdmax=0;
        int sdata_idx=0;
        int *send_buf_len;
        struct timeval tv;
        unsigned int last_buf_addr = 0;
        struct pollfd pfd[1];
        int last_cmd_id;
        mpserver_need_reset = 0;
        last_cmd_id = 0;
        int rebuild_cmd = 0;
        int new_cmd_idx = 0;
        //char ch;
        /*  Accept a connection.  */

        if (client_sockfd != -1)
        {
            printf("MPSERVER: last connection broken, client_sockfd=%d, force close..\n", client_sockfd);
            close(client_sockfd);
            client_sockfd = -1;
        }

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
printf("MPSERVER:  accept client_sockfd=%d\n", client_sockfd);

        if (client_sockfd == -1)
        {
            printf("MPSERVER: %s - Cannot accept: %s\n",__FUNCTION__ , strerror(errno));
            continue;
        }


        printf("MPSERVER: got new connection...\n");
        mpserver_do_init_vqueue();

        fdmax = client_sockfd;
//#ifdef NEW_MPSERVER_CMD
#if 1
        int sbuf, rbuf, len;

        len = sizeof(sbuf);
        sbuf = 16;
        setsockopt(client_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, len);
        len = sizeof(rbuf);
        rbuf = 1024;
        setsockopt(client_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, len);


        len = sizeof(sbuf);
        getsockopt(client_sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, &len);
        len = sizeof(rbuf);
        getsockopt(client_sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
        printf("MPSERVER: client send/receive socket buffer space %d/%d bytes\n", sbuf, rbuf);
#endif

        //tv.tv_usec = 10000;
        //tv.tv_sec = 0;
        memset(&sdata,0,sizeof(sdata));
        while (1)
        {
	    int retry = 10;

            if(retry <= 0){
		printf("MPSERVER: %s:%d retry count down to zero\n", __func__, __LINE__);
		close(client_sockfd);
		client_sockfd = -1;
		break;
            }
	    
            //FD_ZERO(&FD_rcv);
            //FD_SET(client_sockfd, &FD_rcv);

            //tv.tv_usec = 5000;
            //tv.tv_sec = 0;
            //select_ret = select((fdmax+1), &FD_rcv, NULL, NULL, NULL);
            
            pfd[0].fd = client_sockfd;
            pfd[0].events = POLLIN;
            select_ret = poll(pfd,1,-1);

            if (select_ret == 0){
                    printf("MPSERVER: %s:%d select timeout, retry count down %d\n", __func__, __LINE__, retry);
		    retry--;
		    continue;
            }else if (select_ret < 0){
                printf("MPSERVER: %s:%d select got error..  ret=%d   errno=%d\n", __func__, __LINE__, select_ret, errno);
		if(errno == EAGAIN || errno == EINTR){
		    printf("MPSERVER: %s:%d retry count down %d\n", __func__, __LINE__, retry);
		    retry--;
		    continue;
		}else{
                    close(client_sockfd);
		    client_sockfd = -1;
                    break;
		}
            }else{

//                memset(sdata, 0, sizeof(sdata));
                client_len = recv(client_sockfd, sdata, sizeof(sdata)-1,MSG_TRUNC);
	        sdata[client_len] = 0;
                printf("MPSERVER: %s:%d got client_len=%d\n", __func__, __LINE__, client_len);
                if(client_len == 0){
                    printf("MPSERVER: %s:%d - connection closed\n", __func__, __LINE__);
                    close(client_sockfd);
                    client_sockfd = -1;
                    break;
                }else if(client_len < 0){
                    printf("MPSERVER: %s:%d - Cannot recv: %s  client_len=%d\n", __func__, __LINE__, strerror(errno), client_len);
		    if(errno == EAGAIN || errno == EINTR){
			printf("MPSERVER: %s:%d retry count down %d\n", __func__, __LINE__, retry);
			retry--;
			continue;
		    }else{
                        close(client_sockfd);
                        client_sockfd = -1;
                        break;
		    }
                }

                sdata_idx = 0;

                while (client_len > 0)
                {
                    send_buf_len = (int *)&sdata[sdata_idx+4];
                    client_len -= 8;
                    printf("MPSERVER:  cmd=%d  rest client_len=%d  send_buf_len=%d\n", sdata[sdata_idx], client_len, *send_buf_len);
                    switch (sdata[sdata_idx])
                    {
                    case 0x01:
                    {
                        char *saveptr1;
                        char *str1, *str2, *token;
                        int j;
                        int new_pid=0;
//                        send_pkt.Data[0] = mpserver_vdec_wcnts - mpserver_vdec_rcnts;
                        printf("MPSERVER: send_buf_len = %d\n", *send_buf_len);
                        printf("MPSERVER: got str = !%s!\n", &sdata[sdata_idx+8]);

                        for (j = 0, str1 = &sdata[sdata_idx+8];; j++, str1 = NULL) {
                            token = strtok_r(str1, ":", &saveptr1);
                            if (token == NULL)
                                break;
                            NewExecv[j] = strdup(token);
                            printf("%d: %s\n", j, token);
                        }

                        NewExecv[j] = 0;
                        mplayer_pid = 0;
                        new_pid = fork();
                        if (new_pid == 0)
                        {
                            execv(NewExecv[0], NewExecv);
                            exit(0);
                        }
                        mplayer_pid = new_pid;


//                        ret = send(client_sockfd, &send_pkt.Data[0], 1, 0);
//                        fsync(client_sockfd);
                        break;
                    }
                    case 0x02:
                    {
                        char *saveptr1;
                        char *str1, *str2, *token;
                        int j;
                        int new_pid=0;
//                        send_pkt.Data[0] = mpserver_vdec_wcnts - mpserver_vdec_rcnts;
                        printf("MPSERVER: send_buf_len = %d\n", *send_buf_len);
                        printf("MPSERVER: got str = !%s!\n", &sdata[sdata_idx+8]);

                        for (j = 0, str1 = &sdata[sdata_idx+8];; j++, str1 = NULL) {
                            token = strtok_r(str1, ":", &saveptr1);
                            if (token == NULL)
                                break;
                            NewExecv[j] = strdup(token);
                            printf("%d: %s\n", j, token);
                        }

                        NewExecv[j] = strdup("dvdnav://");
                        NewExecv[j+1] = 0;
                        mplayer_pid = 0;
                        new_pid = fork();
                        if (new_pid == 0)
                        {
                            execv(NewExecv[0], NewExecv);
                            exit(0);
                        }
                        mplayer_pid = new_pid;


//                        ret = send(client_sockfd, &send_pkt.Data[0], 1, 0);
//                        fsync(client_sockfd);
                        break;
                    }
                    case 0x03:	//get child status
                    {
                        int status, wait_ret;
                        if (mplayer_pid != 0)
                        {
			    wait_ret = waitpid(mplayer_pid, &status, WNOHANG);
			    if (wait_ret == mplayer_pid)
			    {
				ipc_callback_error(2);
			    }
                            send_pkt.Data[1] = wait_ret;
                            printf("MPSERVER: check mplayer pid=%d, status = %d\n", mplayer_pid, send_pkt.Data[1]);
                            send_pkt.Data[0] = 1;
                        }
                        else
                        {
                            printf("MPSERVER: mplayer is not running..\n");
                            send_pkt.Data[0] = -1;
                        }

                        ret = send(client_sockfd, &send_pkt.Data[0], 2, 0);
                        fsync(client_sockfd);
                        break;
                    }
                    case 0x04:	//kill child
                    {
#if 1
			mpserver_force_kill_child();
#else
                        int status;
                        if (mplayer_pid != 0)
                        {
                            kill(mplayer_pid, 3);
                            usleep(1000*30);
                            kill(mplayer_pid, 3);
                            usleep(1000*30);
                            kill(mplayer_pid, 9);
                            usleep(1000*30);
                            if (mplayer_pid)
			    {
                                waitpid(mplayer_pid, &status, WNOHANG);
			        ipc_callback_error(2);
			    }
                        }
#endif
                        break;
                    }
                    case 0x05:	//rebuild cmd init
                    {
                        int i;
			//try to force kill child before next play
			mpserver_force_kill_child();

                        rebuild_cmd = 1;
                        new_cmd_idx = 0;
                        for (i=0; i<MAX_CMD_IDX; i++)
                        {
                            if (NewExecv[i] != 0)
                            {
                                free(NewExecv[i]);
                            }
                            NewExecv[i] = 0;
                        }
                        break;
                    }
                    case 0x06:	//setup new command
                    {
//                        NewExecv[new_cmd_idx] = strdup(&sdata[sdata_idx+8]);
                        NewExecv[new_cmd_idx] = malloc((*send_buf_len) + 1);
			strncpy(NewExecv[new_cmd_idx], &sdata[sdata_idx+8], *send_buf_len);
			NewExecv[new_cmd_idx][*send_buf_len] = 0;
                        printf("MPSERVER: id 6, idx:%d cmd:%s!\n", new_cmd_idx, NewExecv[new_cmd_idx]);
                        new_cmd_idx++;
//                        ret = send(client_sockfd, &send_pkt.Data[0], 1, 0);
//                        fsync(client_sockfd);
                        break;
                    }
                    case 0x07:	//run new command
                    {
                        int new_pid=0;
                        NewExecv[new_cmd_idx] = 0;

			if(mplayer_pid != 0){
				mpserver_force_kill_child();
				if(mplayer_pid != 0)
					printf("MPSERVER: previous mplayer(%d) has not been killed!\n", mplayer_pid);
			}
                        mplayer_pid = 0;

			            if (mpserver_need_redirect_stdout)
			            {
				            reset_pipe();

				            pipe(mpserver_pipefd_stdout);
				            pipe(mpserver_pipefd_stderr);
			            }
//20110106 Robert add mpserver kill flag
			mpserver_kill_flag = 0;

                        new_pid = fork();
                        if (new_pid == -1) //fork fail
                        {
                            printf("MPSERVER: *****fork fail*****\n");
                        }   
                        
                        if (new_pid == 0) //child process
                        {
			                close(client_sockfd);
			                close(server_sockfd);

			                if (mpserver_need_redirect_stdout)
			                {
				                close(STDOUT_FILENO);
				                dup(mpserver_pipefd_stdout[1]);
				                close(mpserver_pipefd_stdout[0]);
				                mpserver_pipefd_stdout[0] = -1;

				                close(STDERR_FILENO);
				                dup(mpserver_pipefd_stderr[1]);
				                close(mpserver_pipefd_stderr[0]);
				                mpserver_pipefd_stderr[0] = -1;
			                }
//            			    close(STDIN_FILENO);

                            execv(NewExecv[0], NewExecv);
                            exit(0);
                        }
                        mplayer_pid = new_pid;

            			if (mpserver_need_redirect_stdout)
            			{
            				close(STDOUT_FILENO);
            				dup(mpserver_pipefd_stdout[0]);
            				close(mpserver_pipefd_stdout[1]);
            				mpserver_pipefd_stdout[1] = -1;
            
            				close(STDERR_FILENO);
            				dup(mpserver_pipefd_stderr[0]);
            				close(mpserver_pipefd_stderr[1]);
            				mpserver_pipefd_stderr[1] = -1;
            			}


//                        ret = send(client_sockfd, &send_pkt.Data[0], 1, 0);
//                        fsync(client_sockfd);
                        break;
                    }
//todo, not yet finish...
#if 0
		    case 0x11:	//oss volume control
		    {
			    int cmdid=0;
			    int volume_value=0;
			    char *saveptr1;
			    char *str1, *str2, *token;
			    int j;
			    int is_mute=0;
			    printf("MPSERVER: send_buf_len = %d\n", *send_buf_len);
			    printf("MPSERVER: got str = !%s!\n", &sdata[sdata_idx+8]);

			    for (j = 0, str1 = &sdata[sdata_idx+8];; j++, str1 = NULL) {
				    token = strtok_r(str1, ":", &saveptr1);
				    if (token == NULL)
					    break;
				    printf("%d: %s\n", j, token);
				    if (strncmp(token, "set", 3)==0)
				    {
					    //				cmdid = 0x10;
					    j++;
					    token = strtok_r(str1, ":", &saveptr1);
					    if (token == NULL)
						    break;
					    printf("%d: %s\n", j, token);
					    if (strncmp(token, "volumn", 6)==0)
					    {
						    token = strtok_r(str1, ":", &saveptr1);
						    if (token == NULL)
							    break;
						    printf("%d: %s\n", j, token);
						    volumn_value = atoi(token)
							    if (volumn_value >= 0 && volumn_value <= 100)
							    {
								    cmdid = 0x10;
							    }
						    break;
					    }

				    }
				    else if (strncmp(token, "get", 3)==0)
				    {
					    cmdid = 0x20;
				    }
				    else if (strncmp(token, "mute", 3)==0)
				    {
					    cmdid = 0x30;
				    }
			    }

			    printf("MPSERVER: cmdid = %d  volume_value = %d\n", cmdid, volume_value);

			    if (cmdid == 0x10)
			    {
				    ao_control_vol_t *vol = (ao_control_vol_t *)arg;
				    int fd, v, devs;
				    if ((fd = open("/dev/mixer", O_RDONLY)) > 0)
				    {
					    ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devs);
					    if (devs & (1 << oss_mixer_channel))
					    {
						    if (cmd == AOCONTROL_GET_VOLUME)
						    {
							    ioctl(fd, MIXER_READ(oss_mixer_channel), &v);
							    vol->right = (v & 0xFF00) >> 8;
							    vol->left = v & 0x00FF;
							    printf("		   ==> GET  right=%x  left=%x\n", (int)vol->right, (int)vol->left);
						    }
						    else
						    {
							    v = ((int)vol->right << 8) | (int)vol->left;
							    printf("   ==> SET  right=%x  left=%x\n", (int)vol->right, (int)vol->left);

							    ioctl(fd, MIXER_WRITE(oss_mixer_channel), &v);
						    }
					    }
					    else
					    {
						    close(fd);
						    return CONTROL_ERROR;
					    }
					    close(fd);
					    return CONTROL_OK;

				    }

				    //                        ret = send(client_sockfd, &send_pkt.Data[0], 1, 0);
				    //                        fsync(client_sockfd);
				    break;
			    }
		    }
#endif
                    default:
                        printf("MPSERVER: Got Wrong Command... 0x%x  (sdata_idx=%d)\n", sdata[sdata_idx+0], sdata_idx);
                        break;
                    }

                    if (mpserver_need_reset == 1)
                    {
                        close(client_sockfd);
                        client_sockfd = -1;
                        break;
                    }
                    client_len -= *send_buf_len;
                    sdata_idx += *send_buf_len + 8;
                }
//                client_len -= *send_buf_len;
//                sdata_idx += *send_buf_len;
            }

            if (mpserver_need_reset == 1)
            {
                break;
            }
        } //while(1)
    }
}


int main(int argc, char **argv)
{
    int i;

    if (argc == 2)
    {
	if (strncmp(argv[1], "-redirect", 9)==0)
	{
		mpserver_need_redirect_stdout = 1;
	}
    }
printf("mpserver_need_redirect_stdout = %d\n", mpserver_need_redirect_stdout);
#ifdef USE_INSTRUMENT_FUNCTIONS
    memset(g_mpserver_func_parent_list, 0, sizeof(g_mpserver_func_parent_list));
    memset(g_mpserver_func_child_list, 0, sizeof(g_mpserver_func_child_list));
#endif

//    for (i=1; i<=15; i++)
    for (i=1; i<=30; i++)
    {
        signal(i, exit_sighandler);
    }
//    signal(SIGCHLD, exit_sighandler);
    /*
        struct sched_param sp = { .sched_priority = 18 };
        int ret;

    //reset schedule
        ret = sched_setscheduler(getpid(), SCHED_RR, &sp);
    */

    for (i=0; i<MAX_CMD_IDX; i++)
    {
        NewExecv[i] = 0;
    }

    mpserver_pipefd_stdout[0] = -1;
    mpserver_pipefd_stdout[1] = -1;
    mpserver_pipefd_stderr[0] = -1;
    mpserver_pipefd_stderr[1] = -1;


    while (1)
    {
        ipc_handler();
//        usleep(1);
        mpserver_usec_sleep(1000);
    }
    return 0;
}
