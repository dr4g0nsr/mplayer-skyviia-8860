#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "skipc.h"
#include "audio_server.h"


//shared memory address
unsigned char* sm_com = NULL;
unsigned char* sm_in = NULL;
unsigned char* sm_out = NULL;
extern unsigned int aserver_crash;


//return 0: can not find audio server
int wait_server(void *tsc)
{
	int check_num = 100;
	int rv = 0;
	volatile union smc *sc = (union smc *)tsc;
	while ((sc->magic.magic_num != SERVER_MAGIC) && (check_num>0)){
		check_num--;
		usleep(1000);
	}		
	if (sc->magic.magic_num == SERVER_MAGIC)
		rv = 1;
	return rv;		
}

int remove_share_mem(int shmid)
{
#ifndef AD_SHM_REWRITE
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		printf("shmctl(IPC_RMID) failed\n");
		return 0;
	}
#endif
	return 1;		
}

int deallocate_share_mem(void* shared_memory)
{
#ifndef AD_SHM_REWRITE
	if(shmdt(shared_memory) == -1){	
		printf("shmdt failed\n");
		return 0;
	}
#endif
	return 1;
}

void* allocate_share_mem(int shmid)
{
	int rv = (int)shmat(shmid, (void *)0, 0);
	if (rv == -1){
		printf("share memory attach fail\n");
		return 0;
	}
	return (void*)rv; 
}

int get_shared_mem_id(int key, int size)
{
	int sm_id;
   sm_id = shmget((key_t)key, size, 0666|IPC_CREAT);	//get shared memory id, -1 for error
	return sm_id;
}

int get_first_shared_mem_id(int key, int size)
{
	int sm_id;
	sm_id = shmget((key_t)key, size, 0666);
	return sm_id;
}

int get_semaphore_id(void)
{
   int sem_id;
   sem_id = semget((key_t)KEY, 2, 0666|IPC_CREAT);	//get semaphore id
	return sem_id;
}

int reset_semvalue(int sem_id, int idx)
{
	if(semctl(sem_id, idx, SETVAL, 0) == -1){
	   printf("semaphore reset fail\n");
		return 0;
   }		
	return 1;
}

int get_semvalue(int sem_id, int idx)
{
	return semctl(sem_id, idx, GETVAL,0);
}

void del_semvalue(int sem_id, int idx)
{
	union semun sem_union;	
	if(semctl(sem_id, idx, IPC_RMID, sem_union) == -1)
		printf("Failed to delete semaphore\n");
}

int semaphore_p(int sem_id, int idx)
{
	struct sembuf sem_b;	
	sem_b.sem_num = idx;
	sem_b.sem_op = -1;
	//sem_b.sem_flg = SEM_UNDO;
	sem_b.sem_flg = 0;
	while(1){
	   if(semop(sem_id, &sem_b, 1) == -1){
	      if (errno == EINTR)
            continue;
			printf("mplayer: semaphore_p failed errno=%d\n", errno);
			return 0;
      }	
      break;		
   }
	return 1;
}

int semaphore_v(int sem_id, int idx)
{
	struct sembuf sem_b;	
	sem_b.sem_num = idx;
	sem_b.sem_op = 1;
	//sem_b.sem_flg = SEM_UNDO;
	sem_b.sem_flg = 0;
	while(1){
	   if(semop(sem_id, &sem_b, 1) == -1){
			if (errno == EINTR)
            continue;
			printf("mplayer: semaphore_v failed errno=%d\n", errno);
			return 0;
		}
		break;
   }		
	return 1;
}

void get_2_semv(int id, int *v0, int *v1)
{
	*v0 = get_semvalue(id, 0);
	*v1 = get_semvalue(id, 1);
}
//return 0: error
int ipc_init(union smc *sc, int codec_id, int data_len, int id, int shmid, int shmid_len)
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

int ipc_decode(union smc *sc, int codec_id, int data_len, int id)
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

int ipc_uninit(union smc *sc, int codec_id, int id)
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

int ipc_ctrl(union smc *sc, int codec_id, int command, int id)
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






