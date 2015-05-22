 
#ifndef _C2MS_H_
#define _C2MS_H_
 
#ifdef _cplusplus
extern "C" {
#endif
 
void * c2ms_mgmt_init  (uint8 * confile, uint64 uid, uint8 * hid, int cachesize);
int    c2ms_mgmt_clean (void * vmgmt);

void * c2ms_mem_alloc    (void * vmgmt, int size);
int    c2ms_mem_free     (void * vmgmt, void * pbuf); 
int    c2ms_mem_restsize (void * vmgmt); 

uint64  c2ms_uid    (void * vmgmt);
uint8 * c2ms_hid    (void * vmgmt);
uint8 * c2ms_localip(void * vmgmt); 
uint8 * c2ms_natip  (void * vmgmt);
uint32  c2ms_oemid     (void * vmgmt);
uint32  c2ms_version   (void * vmgmt);
time_t  c2ms_buildtime (void * vmgmt);


void * c2ms_sess_open   (void * vmgmt, uint8 * url, int * exitcode);
int    c2ms_sess_close  (void * vsess);
 
int c2ms_sess_attr      (void * vsess, uint8 * fid, uint8 * mname, uint64 * msize, uint8 * mmime);
int c2ms_sess_idxfile   (void * vsess, uint8 * idxfid, uint32 * idxlen);
     
int c2ms_sess_cache_init(void * vsess, int packsize);

int c2ms_sess_range     (void * vsess, uint64 offset, uint64 length);
int c2ms_sess_set_media (void * vsess, int idxflag, uint64 offset, uint64 length);
int c2ms_sess_seek      (void * vsess, uint64 seekpos);
int c2ms_sess_read      (void * vsess, void * pbuf, int length, int nbmode);
int c2ms_sess_recv      (void * vsess, void * pbuf, int length, int waitms);
 
int c2ms_sess_idx_open  (void * vsess, uint32 * idxsize);
int c2ms_sess_idx_open_asyn (void * vsess, uint32 * idxsize);
int c2ms_sess_idx_read  (void * vsess, uint8 * pbuf, int length, uint32 offset);
int c2ms_sess_idx_close (void * vsess);
int c2ms_sess_idx_offset(void * vsess, uint64 * poffset);

int c2ms_sess_buffering (void * vsess, int bufsize);
int c2ms_sess_bufratio  (void * vsess, int * ratio);

double c2ms_sess_realtime_recvspeed (void * vsess);
double c2ms_sess_average_recvspeed (void * vsess);

char * c2ms_sess_error  (int errcode);


#ifdef _cplusplus
}
#endif
 
#endif
 
