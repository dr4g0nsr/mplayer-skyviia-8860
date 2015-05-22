#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#define SOCK_NAME       " skynet"

#define SKYNET_CLI_MAGIC        0xCAFE0000
#define SKYNET_SER_MAGIC        0xF00D0000

#define SKYNET_RESP_OK          (SKYNET_SER_MAGIC | 0x0)
#define SKYNET_RESP_ERROR       (SKYNET_SER_MAGIC | 0x1)
#define SKYNET_RESP_UNKNOWN     (SKYNET_SER_MAGIC | 0x2)

#define SKYNET_CMD_INIT         (SKYNET_CLI_MAGIC | 0x0)
#define SKYNET_CMD_GETDATA      (SKYNET_CLI_MAGIC | 0x1)
#define SKYNET_CMD_SETURL       (SKYNET_CLI_MAGIC | 0x2)
#define SKYNET_CMD_GETSIZE      (SKYNET_CLI_MAGIC | 0x3)
#define SKYNET_CMD_GETSEEKABLE  (SKYNET_CLI_MAGIC | 0x4)
#define SKYNET_CMD_SETPOSITION  (SKYNET_CLI_MAGIC | 0x5)
#define SKYNET_CMD_STARTSTREAM  (SKYNET_CLI_MAGIC | 0x6)
#define SKYNET_CMD_STOPSTREAM   (SKYNET_CLI_MAGIC | 0x7)
#define SKYNET_CMD_GETSTREAMTYPE	(SKYNET_CLI_MAGIC | 0x8)
#define SKYNET_CMD_SEEKTOTIME	(SKYNET_CLI_MAGIC | 0x9)

#define SKYNET_CMD_LEN          512     //command
#define SKYNET_RESP_LEN         512     //response
#define SKYNET_DATA_LEN         1024*128

enum {
	SKYNET_STREAM_RMVB,
	SKYNET_STREAM_WMV,
	SKYNET_STREAM_FLV
};

int connection_alive = 0;

char recvbuf[SKYNET_CMD_LEN];
char sendbuf[SKYNET_RESP_LEN];
char databuf[SKYNET_DATA_LEN];

uint32_t *recvval = (uint32_t *)recvbuf;
uint32_t *sendval = (uint32_t *)sendbuf;

static void dump_buf(off_t ofs, unsigned char *buf, int size)
{
	int i;

	for(i=0; i<size; i++){
		if((ofs + i) % 16 == 0)
			printf("\n%0llx: ", ofs + i);
		printf("%02x ", *(buf + i));
	}
	printf("\n");
}

static void signal_handler(int sig)
{
	if(sig == SIGPIPE){
		printf("%s: broken pipe\n", __func__);
		connection_alive = 0;
		return;
	}
}

static int recv_data(int fd, void *buf, size_t len)
{
	int ret;
	
	ret = recv(fd, buf, len, 0);
	if(ret == -1){
		printf("%s: receive error: %s\n", __func__, strerror(errno));
	}else if(ret == 0){
		printf("%s: connection closed by peer\n", __func__);
		connection_alive = 0;
	}
	return ret;
}

static int connection_handler(int connection_fd, char *fn)
{
	int fd;
	int ret;
	int nbytes;
	ssize_t readsize, sendsize;
	char buffer[256];
	uint32_t reqsize, availsize;
	uint64_t newtime;
	off_t newpos;
	off_t reqpos;
	struct stat fst;
	struct pollfd pollst;

#if 0
	nbytes = read(connection_fd, buffer, 256);
	buffer[nbytes] = 0;

	printf("MESSAGE FROM CLIENT: %s\n", buffer);
	nbytes = sprintf(buffer, "hello from the server");
	write(connection_fd, buffer, nbytes);
#endif

	if(fn){
		fd = open(fn, O_RDONLY);
		if(fd == -1){
			printf("%s: failed to open file %s (%s)\n", __func__, fn, strerror(errno));
			close(connection_fd);
			return 1;
		}

		if(fstat(fd, &fst) == -1){
			printf("%s: failed to get file status on %s %s\n", __func__, fn, strerror(errno));
			close(fd);
			close(connection_fd);
			return 1;
		}
	}

	while(connection_alive){
		ret = recv_data(connection_fd, recvbuf, sizeof(uint32_t));
		 if(ret < sizeof(uint32_t)){
			printf("invalid command length %d\n", ret);
			break;
		}

		//printf("%s: receive command %x len %d\n", __func__, recvval[0], ret);
		switch(recvval[0]){
		case SKYNET_CMD_INIT:
			sendval[0] = SKYNET_RESP_OK;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			break;
		case SKYNET_CMD_SETURL:
			ret = recv_data(connection_fd, recvbuf, SKYNET_CMD_LEN);
			if(ret <= 0){
				printf("%s: failed to get url parameter\n", __func__);
				break;
			}
			fn = (char *)recvval;
			//printf("%s: filename %s\n", __func__, fn);
			fd = open(fn, O_RDONLY);
			if(fd == -1){
				printf("%s: failed to open file %s (%s)\n", __func__, fn, strerror(errno));
				sendval[0] = SKYNET_RESP_ERROR;
				send(connection_fd, sendbuf, sizeof(uint32_t), 0);
				break;
			}
			if(fstat(fd, &fst) == -1){
				printf("%s: failed to get file status on %s %s\n", __func__, fn, strerror(errno));
				close(fd);
				sendval[0] = SKYNET_RESP_ERROR;
				send(connection_fd, sendbuf, sizeof(uint32_t), 0);
				return 1;
			}
			sendval[0] = SKYNET_RESP_OK;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			break;
		case SKYNET_CMD_GETSIZE:
			sendval[0] = SKYNET_RESP_OK;
			sendval[1] = fst.st_size & 0xFFFFFFFF;
			sendval[2] = (fst.st_size >> 32) & 0xFFFFFFFF;
			send(connection_fd, sendbuf, sizeof(uint32_t) * 3, 0);
			break;
		case SKYNET_CMD_GETSEEKABLE:
			sendval[0] = SKYNET_RESP_OK;
			sendval[1] = 1;
			send(connection_fd, sendbuf, sizeof(uint32_t) * 2, 0);
			break;
		case SKYNET_CMD_GETSTREAMTYPE:
			sendval[0] = SKYNET_RESP_OK;
			sendval[1] = SKYNET_STREAM_FLV;
			send(connection_fd, sendbuf, sizeof(uint32_t) * 2, 0);
			break;
		case SKYNET_CMD_SEEKTOTIME:
			newtime = ((off_t)recvval[1] << 32) | recvval[0];
			printf("seek to time %lld\n", newtime);

			newpos = 0x2E4C000;
			sendval[0] = SKYNET_RESP_OK;
			sendval[1] = newpos & 0xFFFFFFFF;
			sendval[2] = (newpos >> 32) & 0xFFFFFFFF;
			send(connection_fd, sendbuf, sizeof(uint32_t) * 3, 0);

		case SKYNET_CMD_SETPOSITION:
			ret = recv_data(connection_fd, recvbuf, sizeof(uint32_t)*2);
			if(ret < sizeof(uint32_t)*2){
				printf("%s: failed to get position parameter\n", __func__);
				break;
			}
			reqpos = ((off_t)recvval[1] << 32) | recvval[0];
			//printf("%s reqpos:%llx\n", __func__, reqpos);

			if(reqpos >= fst.st_size){
				printf("%s reqpos %llx >= st_size %llx\n", __func__, reqpos, fst.st_size);
				goto end_of_connection;
			}

			lseek(fd, reqpos, SEEK_SET);
			sendval[0] = SKYNET_RESP_OK;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			break;
		case SKYNET_CMD_STARTSTREAM:
			sendval[0] = SKYNET_RESP_OK;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			
			readsize = 0;
			while(1){
				// polling for read / write
				memset(&pollst, 0, sizeof(pollst));
				pollst.fd = connection_fd;
				pollst.events = POLLIN | POLLOUT;
				ret = poll(&pollst, 1, 1000);
				//printf("ret %d revents %x POLLIN %x POLLOUT %x\n", ret, pollst.revents, POLLIN, POLLOUT);
				if(ret == -1){
					printf("error on polling: %d(%s)\n", errno, strerror(errno));
					goto end_of_connection;
				}else if(ret == 0){
					//printf("poll timeout\n");
					continue;
				}else if(pollst.revents & POLLIN){
					//printf("new command arrives, stop transmission\n");
					break;
				}else if(pollst.revents & POLLOUT){
					// read more data
					if(readsize == 0){
						sendsize = 0;
						readsize = read(fd, databuf, SKYNET_DATA_LEN);
						if(readsize == -1){
							printf("failed to read file %s\n", strerror(errno));
							goto end_of_connection;
						}else if(readsize == 0){
							printf("end of file\n");
							//goto end_of_connection;
							break;
						}
						//dump_buf(lseek(fd, 0, SEEK_CUR) - readsize, databuf, 10240);
					}

					// send remaining data
					ret = send(connection_fd, databuf + sendsize, readsize, MSG_DONTWAIT);
					if(ret == -1){
						if(errno == EAGAIN){
							printf("%s: send fail, try again\n", __func__);
							continue;
						}
						printf("send data failed: %d(%s)\n", errno, strerror(errno));
						goto end_of_connection;
					}
					//printf("%s: send length %d\n", __func__, ret);
					readsize -= ret;
					sendsize += ret;
				}
			}
			break;
		case SKYNET_CMD_STOPSTREAM:
			// client may not receive OK response because there's still data in buffer
			sendval[0] = SKYNET_RESP_OK;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			break;
#if 0
		case SKYNET_CMD_GETDATA:
			ret = recv_data(connection_fd, recvbuf, sizeof(uint32_t)*3);
			if(ret < sizeof(uint32_t)*3){
				printf("%s: failed to get GETDATA parameters\n", __func__);
				break;
			}
			reqsize = recvval[0];
			reqpos = ((off_t)recvval[2] << 32) | recvval[1];
			//printf("%s reqsize:%d reqpos:%llx\n", __func__, reqsize, reqpos);

			if(reqpos >= fst.st_size)
				availsize = 0;
			else if(fst.st_size - reqpos < reqsize)
				availsize = fst.st_size - reqpos;
			else
				availsize = reqsize;

			if(availsize > SKYNET_DATA_LEN)
				availsize = SKYNET_DATA_LEN;

			if(availsize){
				lseek(fd, reqpos, SEEK_SET);
				readsize = read(fd, databuf, availsize);
				if(readsize == -1){
					printf("failed to read file %s\n", strerror(errno));
					goto end_of_connection;
				}else if(readsize != availsize){
					printf("read size mismatch, expect:%d actual:%d\n", availsize, readsize);
					availsize = readsize;
				}
			}
			if(availsize == 0){
				printf("end of file\n");
				goto end_of_connection;
			}
			//printf("get data request:%d available:%d\n", reqsize, availsize);
			send(connection_fd, databuf, availsize, 0);
			break;
#endif
		default:
			printf("Unknown command %x\n", recvval[0]);
			sendval[0] = SKYNET_RESP_UNKNOWN;
			send(connection_fd, sendbuf, sizeof(uint32_t), 0);
			break;
		}
	}

end_of_connection:
	close(fd);
	close(connection_fd);
	return 0;
}

int main(int argc, char *argv[])
{
	struct sockaddr_un address;
	int socket_fd, connection_fd;
	size_t address_length;
	pid_t child;
	char *fn = NULL;

	/*
	   if(argc != 2){
	   printf("Usage: %s [filename]\n", argv[0]);
	   return 1;
	   }
	 */

	if(argc >= 2)
		fn = argv[1];

	// Install signal handler
	signal(SIGPIPE, signal_handler);

	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socket_fd < 0){
		printf("socket() failed\n");
		return 1;
	} 

	//unlink("./demo_socket");
	address.sun_family = AF_UNIX;
	address_length = sizeof(address.sun_family) + sprintf(address.sun_path, SOCK_NAME);
	address.sun_path[0] = 0;   //Use abstract name as domain socket

	if(bind(socket_fd, (struct sockaddr *) &address, address_length) != 0){
		printf("bind() failed\n");
		return 1;
	}

	if(listen(socket_fd, 5) != 0){
		printf("listen() failed\n");
		return 1;
	}

	while((connection_fd = accept(socket_fd, (struct sockaddr *) &address, &address_length)) > -1){
		connection_alive = 1;
		connection_handler(connection_fd, fn);
	}

	close(socket_fd);
	//unlink("./demo_socket");
	return 0;
}

