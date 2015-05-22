#ifndef _DBG_TOOL_H_
#define _DBG_TOOL_H_

#include <time.h>
#define DUMP_FILENAME "/mnt/usb81/dump/%Y%m%d-%H%M%S.dump"
inline void dump_to_file(int *fd, char *buf, int len)
{
	if(*fd == 0){
		char dumpfn[128];
		time_t tt = time(NULL);
		strftime(dumpfn, 128, DUMP_FILENAME, localtime(&tt));
		*fd = open(dumpfn, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if(*fd == -1){
			printf("%s:%d failed to open file %s\n", __func__, __LINE__, dumpfn);
		}else{
			printf("%s:%d open file for write: %s\n", __func__, __LINE__, dumpfn);
		}
	}

	if(*fd > 0){
		if(write(*fd, buf, len) != len){
			printf("%s:%d failed to write %d bytes to file, %s\n", __func__, __LINE__, len, strerror(errno));
			close(*fd);
			*fd = -1;
		}else{
			//printf("%s:%d write %x bytes to file success\n", __func__, __LINE__, len);
		}
	}
}

#endif

