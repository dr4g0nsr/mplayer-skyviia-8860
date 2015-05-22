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
    printf("get cur pos :\tclient c g\n");
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
    int reuse=1;
    int should_send=1;
    int sbuf, rbuf, len;

    if (argc < 3)
    {
        print_help();
        return;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;

    server_len = sizeof(serv_addr.sun_family) +
                 sprintf(serv_addr.sun_path, " mpserversock");

    serv_addr.sun_path[0] = 0;   //Use abstract name as domain socket
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    result = connect(sockfd, (struct sockaddr *)&serv_addr, server_len);

    len = sizeof(sbuf);
    sbuf = 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sbuf, len);
    len = sizeof(rbuf);
    rbuf = 16;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rbuf, len);


    if (!result)
        printf("Connect Success\n");

    printf("argv[2]=%s!\n", argv[2]);
    strcpy(&sendbuf[8], argv[2]);
    printf("sendbuf[%s]\n", &sendbuf[8]);
    printf("sendbuf len=%d\n", strlen(argv[2]));
    unsigned int *iptr = (unsigned int *)sendbuf;
    iptr[0] = atoi(argv[1]);
    if (iptr[0] == 5)
    {
        iptr[0] = 5;
        sprintf(&sendbuf[8], "0");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 6;
        sprintf(&sendbuf[8], "/system/bin/mplayer");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 6;
        sprintf(&sendbuf[8], "-skydroid");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 6;
        sprintf(&sendbuf[8], "-forcelock");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 6;
        sprintf(&sendbuf[8], "-quiet");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 6;
//	sprintf(&sendbuf[8], "/mnt/usb1/Cal-Comp/philips.demo.colors.of.miami.1080i.ts");
        sprintf(&sendbuf[8], "/mnt/usb1/Cal-Comp/bbc-japan_1080p.mkv");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        iptr[0] = 7;
        sprintf(&sendbuf[8], "0");
        iptr[1] = strlen(&sendbuf[8]);
        iptr[1] = ((iptr[1]+3)>>2)<<2;
        result = send(sockfd,sendbuf, iptr[1]+8, 0);

        return;
    }

    iptr[1] = strlen(argv[2]);
    result = send(sockfd,sendbuf, strlen(argv[2])+8, 0);
    if (result == -1)
        printf("%d - %s - Cannot send: %s\n",__LINE__,__FUNCTION__ , strerror(errno));

    if (iptr[0] == 3)
    {
        printf("mpclient: wait status return...\n");
        result = recv(sockfd, recvbuf, 16, MSG_TRUNC);
        printf("mpclient: got return len:%d, recvbuf[0]=%d  recvbuf[1]=%d\n", result, recvbuf[0], recvbuf[1]);
    }

    close(sockfd);

    return 0;
}