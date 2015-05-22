
#define S_DEBUG 

#define KEY (3555)
#define KEY2 (3556)
#define KEY_SM_COM (3555)
#define KEY_SM_IN (3556)
#define KEY_SM_OUT (3557)


union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int get_semaphore_id2(void);
int get_semaphore_id(void);
int reset_semvalue(int sem_id, int idx);        
int get_semvalue(int sem_id, int idx);
void del_semvalue(int sem_id, int idx);
int semaphore_p(int sem_id, int idx);
int semaphore_v(int sem_id, int idx);
void get_2_semv(int id, int *v0, int *v1);
int get_shared_mem_id(int key, int size);
int get_first_shared_mem_id(int key, int size);
void* allocate_share_mem(int shmid);
int deallocate_share_mem(void* shared_memory);
int wait_server(void *tsc);
unsigned int aserver_pid;







