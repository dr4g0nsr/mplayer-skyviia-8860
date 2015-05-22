#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "domain_socket.h"


void print_help()
{
    printf("pause    :\tclient p s\n");
    printf("quit     :\tclient q s\n");
    printf("seek +10s:\tclient f s\n");
    printf("seek -10s:\tclient r s\n");
    printf("mute     :\tclient m s\n");
    printf("volume  +:\tclient \\* s\n");
    printf("volume  -:\tclient / s\n");
    printf("audio switch:\tclient a s\n");
    printf("subtitle up:\tclient s1 s\n");
    printf("subtitle down:\tclient s2 s\n");
    printf("cmd      :\tclient c s \"str\"\n");
    
    printf("get duration:\tclient d g\n");
    printf("get cur pos :\tclient c g 0\n");
    printf("get cur pos callback on:\tclient c g 1\n");
    printf("get cur pos callback off:\tclient c g 2\n");
    printf("get buffering status :\tclient f g 0\n");
    printf("get buffering status callback on:\tclient f g 1\n");
    printf("get buffering status callback off:\tclient f g 2\n");
    printf("get status  :\tclient p g\n");
    printf("get cur volume:\tclient v g\n");
    return;
}

int main(int argc, char *argv[])
{
int sockfd;
struct	sockaddr_un serv_addr;
int result;
char sendbuf[1024];
unsigned char recvbuf[1024];
GeneralPkt	gen;
pGeneralPkt	pgen;
int ret;
void *recv_ptr;
void *send_ptr;
int		server_len;
int should_send=1;

if(argc < 3)
{
    print_help();
    return;
}

	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    
    server_len = sizeof(serv_addr.sun_family) +
                  sprintf(serv_addr.sun_path, " mplayersock");

    serv_addr.sun_path[0] = 0;   //Use abstract name as domain socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	result = connect(sockfd, (struct sockaddr *)&serv_addr, server_len);
	if(!result)
		printf("Connect Success\n");

if(argv[2][0] == 's')
{
	memset(&gen,0,sizeof(GeneralPkt));
	send_ptr = &gen;
	if(argv[1][0] == 'p')
	{
		gen.Control = SET;
		gen.Subtype = PLAY_PROC | PLAY_PAUSE;
	}
	else if(argv[1][0] == 'q')
	{
		gen.Control = SET;
		gen.Subtype = PLAY_PROC | PLAY_STOP;
	}
	else if(argv[1][0] == 'f')
	{
		gen.Control = SET;
		gen.Subtype = SEEK_PROC | SEEK_FF_10_SEC;
	}
	else if(argv[1][0] == 'r')
	{
		gen.Control = SET;
		gen.Subtype = SEEK_PROC | SEEK_FB_10_SEC;
	}
	else if(argv[1][0] == 'm') //mute
	{
		gen.Control = SET;
		gen.Subtype = VOLUME_PROC | VOLUME_MUTE;
	}
	else if(argv[1][0] == '/') //Decrease volume.
	{
		gen.Control = SET;
		gen.Subtype = VOLUME_PROC | VOLUME_DOWN;
	}
	else if(argv[1][0] == '*') //increase volume.
	{
		gen.Control = SET;
		gen.Subtype = VOLUME_PROC | VOLUME_UP;
	}
	else if(argv[1][0] == 'a') //audio switch
	{
		gen.Control = SET;
		gen.Subtype = AUDIO_PROC | AUDIO_UP;
	}
	else if(argv[1][0] == 's' && argv[1][1] == '1') //sub up
	{
        printf("sub up\n");
		gen.Control = SET;
		gen.Subtype = SUB_PROC | SUB_UP;
	}
	else if(argv[1][0] == 's' && argv[1][1] == '2') //sub down
	{
        printf("sub down\n");
		gen.Control = SET;
		gen.Subtype = SUB_PROC | SUB_DOWN;
	}
	else if(argv[1][0] == 'c') //command instruction
	{
        printf("cmd instruction\n");
//		gen.Control = SET;
//		gen.Subtype = CMD_PROC | CMD_INST;
//		strcpy(gen.CmdInst,argv[3]);
		strcpy(sendbuf,argv[3]);
printf("sendbuf[%s]\n",sendbuf);
	    result = send(sockfd,sendbuf, strlen(sendbuf), 0);
	    if(result == -1)
            printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));
        should_send = 0;
        return;
	}
	
	if(should_send)
	    send(sockfd,send_ptr, sizeof(gen), 0);

	printf("control[%d]\n",gen.Control);
	printf("class[%x]\n",gen.Subtype & 0xf0 >>4);
	printf("value[%x]\n",gen.Subtype & 0x0f);

	if(argv[1][0] == 'a' || (argv[1][0] == 's' && argv[1][1] == '1') || (argv[1][0] == 's' && argv[1][1] == '2')) //audio switch | sub up | sub down
	{
        ret = recv(sockfd,recvbuf,1024,0);
		printf("recvbuf[%x]-len[%d]\n",recvbuf[0],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[1],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[2],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[3],sizeof(recvbuf));
    }
}
else if(argv[2][0] == 'g')
{
		send_ptr = &gen;
		if(argv[1][0] == 'd')
	        {
    	            gen.Control = GET;
		    gen.Subtype = GET_DURATION_PROC | GET_DURATION_SEC;
		}
		else if(argv[1][0] == 'c')
		{
                    if(argv[3][0] == '0')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_CURRENT_POS_PROC | GET_CURRENT_POS_SEC;
		    }
		    else if(argv[3][0] == '1')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_CURRENT_POS_PROC | GET_CURRENT_POS_SEC_CALL_BACK_ON;
		    }
		    else if(argv[3][0] == '2')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_CURRENT_POS_PROC | GET_CURRENT_POS_SEC_CALL_BACK_OFF;
		    }
		}
		else if(argv[1][0] == 'f')
		{
                    if(argv[3][0] == '0')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_BUFFERING_STATUS_PROC | GET_BUFFERING_PERCENTAGE;
		    }
		    else if(argv[3][0] == '1')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_BUFFERING_STATUS_PROC | GET_BUFFERING_CALL_BACK_ON;
		    }
		    else if(argv[3][0] == '2')
                    {
		        gen.Control = GET;
		        gen.Subtype = GET_BUFFERING_STATUS_PROC | GET_BUFFERING_CALL_BACK_OFF;
		    }
		}
		else if(argv[1][0] == 'p')
		{
		    gen.Control = GET;
		    gen.Subtype = GET_PLAYER_STATUS_PROC | PLAYER_STATUS_PLAYING;
		}
		else if(argv[1][0] == 'v')
		{
		    gen.Control = GET;
		    gen.Subtype = GET_AUDIOINFO_PROC | GET_CURRENT_AUDIO_VOLUME;
		}

		ret = send(sockfd,send_ptr, 1024, 0);

printf("ret[%d]\n",ret);
printf("control[%d]\n",gen.Control);
printf("Subtype[%02x]\n",gen.Subtype);

		ret = recv(sockfd,recvbuf,1024,0);
		printf("recvbuf[%x]-len[%d]\n",recvbuf[0],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[1],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[2],sizeof(recvbuf));
		printf("recvbuf[%x]-len[%d]\n",recvbuf[3],sizeof(recvbuf));
}
	close(sockfd);	

    return 0;
}
