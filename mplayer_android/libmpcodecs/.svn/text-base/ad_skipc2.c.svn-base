#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "skipc.h"
#include "audio_server2.h"


//shared memory address
unsigned char* sm_com;
unsigned char* sm_in;
unsigned char* sm_out;
extern unsigned int aserver_crash;


int get_semaphore_id2(void)
{
   int sem_id;
   sem_id = semget((key_t)KEY2, 2, 0666|IPC_CREAT);	//get semaphore id
	return sem_id;
}

//return 0: error
int ipc2_init(union smc *sc, int codec_id, int data_len, int id, int shmid, int shmid_len)
{
	sc->pinit.shmid = shmid;
	sc->pinit.shmid_len = shmid_len;
	sc->pinit.com_id = PLAYER_INIT;
	sc->pinit.codec_id = codec_id;
	sc->pinit.data_len = data_len;
	
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return 0;
	}
	
	if (!semaphore_v(id, 1))
		return 0;				
	if (!semaphore_p(id, 0))
		return 0;
	
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return 0;
	}
	return 1;	
}

int ipc2_decode(union smc *sc, int codec_id, int data_len, int id)
{
	sc->pdecod.com_id = PLAYER_DECODE;
	sc->pdecod.codec_id = codec_id;
	sc->pdecod.data_len = data_len;
	
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return -1;
	}
	
	if (!semaphore_v(id, 1))
		return 0;				
	if (!semaphore_p(id, 0))
		return 0;
	
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return -1;
	}
	return 1;	
}

int ipc2_uninit(union smc *sc, int codec_id, int id)
{
	sc->pctrl.com_id = PLAYER_UNINIT;
	sc->pctrl.codec_id = codec_id;
	
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return 0;
	}
	
	if (!semaphore_v(id, 1))
		return 0;				
	if (!semaphore_p(id, 0))
		return 0;
	return 1;	
}

int ipc2_ctrl(union smc *sc, int codec_id, int command, int id)
{
	sc->pctrl.com_id = PLAYER_CTRL;
	sc->pctrl.codec_id = codec_id;
	sc->pctrl.command = command;
	if(aserver_pid != 0 && sc->pinit.pid != aserver_pid)
	{
		aserver_crash = 1;
		return 0;
	}
	
	if (!semaphore_v(id, 1))
		return 0;				
	if (!semaphore_p(id, 0))
		return 0;
	return 1;	
}






