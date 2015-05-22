
#ifndef _SALLOC_H_
#define _SALLOC_H_

extern void *SallocVirtualBase;
extern void *SallocPhysicalBase;
extern void *PFMVirtualBase;
extern void *PFMPhysicalBase;

extern unsigned int SallocBufferSize;

#define VIRTUAL_TO_UCM(buffer)  ( (unsigned)buffer - ((char *)SallocVirtualBase - (char *)SallocPhysicalBase))


void salloc_init(void *mem, int len);
void salloc_uninit(void);

void *salloc(int len);

void sfree(void *mem);

void *srealloc(void *mem, int len);

#endif
