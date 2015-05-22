/* Simple malloc clone
 * amnon June-06
 * 
 * This allocator is used to manage the allocation of the uncached memory
 * region used by VLC DMA assist.
 */
#include "salloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned *free_list;
static unsigned total_size;
static unsigned *salloc_addr;	// Raymond 2008/03/21

// Raymond 2008/03/06
//#define DEBUG_SALLOC

unsigned int salloc_size = 0;	// Raymond 2008/10/22

#ifdef DEBUG_SALLOC
static unsigned int salloc_cnt;	
#endif

static void *trim(void *addr, int len);
static void _sfree(void *addr);

#undef NULL
#define NULL 0

// Raymond 2010/01/18
#define LEN_BYTES	8		// byte alignment

#if LEN_BYTES == 4
#define LEN(p) ((long *)p)[-1]
#else if LEN_BYTES == 8
#define LEN(p) ((long *)p)[-2]
#endif

#ifdef TRACE_SALLOC
/* define me if you want allocation tracing */

typedef struct tr_t {
    struct tr_t *next;
    void *addr;
    int len;
    char *file;
    int lineno;
    int alloced_size;
    int resize_size;
    int seq_no;
} tr_t;

static struct tr_t *active = NULL;
static int seq_counter = 0;

static void tr_add(void *addr, int len)
{
    tr_t *t = malloc(sizeof(tr_t));

    memset(t, 0, sizeof(tr_t));
    t->next = active;
    t->addr = addr;
    t->len = len;
    t->alloced_size = len;
    t->seq_no = seq_counter++;
    active = t;
}

static void tr_del(void *addr)
{
    tr_t **p, *tmp;

    for (p=&active; *p && (*p)->addr != addr; p = &((*p)->next));

    if (!*p)
    {
	printf("attempt to free %p, which was never allocated!\n", addr);
	return;
    }

    tmp = *p;
    *p = tmp->next;
    free(tmp);
}

static void tr_change(void *addr, int new_len)
{
    tr_t *p;

    for (p = active; p && p->addr != addr; p=p->next);

    if (!p)
	return;
    p->len = new_len;
}

static void print_contents(unsigned char *p)
{
    int i;
    for (i=0; i<4; i++)
	printf("%02x ", p[i]);
}

static void tr_print_free_list(void)
{
    unsigned *p;
    unsigned free_size = 0;

    printf("Total size = %d\n", total_size);

    for (p = free_list; p; p = (void *)*p)
	printf("%p len %ld\n", p, LEN(p));
    printf("\n");
}

static void tr_print(void)
{
    tr_t *p;
    int s = 0;

    printf("active\n");
    for (p = active; p; p=p->next)
    {
	printf("%s:%d %p, len=%d A %d R %d seq %d", p->file, p->lineno, p->addr, p->len,
	p->alloced_size, p->resize_size, p->seq_no);
	print_contents(p->addr);
	printf("\n");

	s += p->len;
    }
    printf("Total active %d\n", s);
    tr_print_free_list();
}

#else

/* dummy stubs */
static void tr_add(void *addr, int len) {}
static void tr_del(void *addr) {}
static void tr_change(void *addr, int new_len) {}
static void tr_print(void) {}
#endif


void salloc_init(void *mem, int len)
{
    unsigned *p = mem;

    /* put all memory on free list */
//    p++;
    p += (LEN_BYTES >> 2);	// Raymond 2010/01/18
    LEN(p) = len;
    _sfree(p);

    total_size = len;
    salloc_addr = mem;
#ifdef DEBUG_SALLOC		// Raymond 2008/03/06
	printf("salloc_init: mem = %X\n", (unsigned int)salloc_addr);
	printf("salloc_init: total_size = %d\n", len);
#endif
}

void salloc_uninit(void)
{
    free_list = NULL;
}

static void out_of_mem(int len)
{
    printf("out uncached memory (tried to alloc %d bytes)\n", len);
    printf("Re-compile Kernel with larger unchached memory.\n");
    printf("Current uncached size = %d bytes\n", total_size);
    tr_print();
    exit(1);
}

static int roundup(int n)
{
	// Raymond 2010/01/18
	int l = LEN_BYTES - 1;
	return (n + l) & ~l;
//    return (n + 3) & ~3;
}

static unsigned *best_fit(int len)
{
    unsigned *p, *best_p = NULL;
    unsigned min_len = ~0;

    for (p = free_list; p; p = (void*)*p)
    {
	if (LEN(p) < len)
	    continue;
	if (LEN(p) >= min_len)
	    continue;
	best_p = p;
	min_len = LEN(p);
    }
    return best_p;
}

void *salloc(int len)
{
    unsigned **p;
    unsigned *ret;

    len = roundup(len);

    ret = best_fit(len);
    if (!ret)
	out_of_mem(len);

    for (p = &free_list; *p != ret; p = (void*)*p);
    *p = (void *)*ret; /* remove from free list */

    tr_add(ret, LEN(ret));
    trim(ret, len);

	salloc_size += len;
#ifdef DEBUG_SALLOC		// Raymond 2008/03/06	
	salloc_cnt++;
	printf("salloc: addr %8X - len = %6d / %d (%2d)\n", (unsigned int)ret, len, salloc_size, salloc_cnt);
#endif

    return ret;
}

static int is_next_to(void *left, void *right)
{
    return (char *)left + LEN(left) + LEN_BYTES == right;
}

static void *join(void *left, void *right)
{
    LEN(left) += LEN(right) + LEN_BYTES;
    return left;
}

static void _sfree(void *addr)
{
    unsigned **p, *next;

    /* try to merge with neigbouring blocks */
    for (p = &free_list; *p; p = (void *)next)
    {
	next = *p;
	if (is_next_to(*p, addr))
	{
	    *p = (void *)*next;
	    *next = NULL;
	    addr = join(next, addr);
	}
	else if (is_next_to(addr, *p))
	{
	    *p = (void *)*next;
	    *next = NULL;
	    addr = join(addr, next);
	}
    }

    /* insert head of list */
    *(unsigned *)addr = (unsigned)free_list;
    free_list = addr;
}

void sfree(void *addr)
{
	// Raymond 2008/03/21
	if((unsigned *)addr < salloc_addr)
	{
#ifdef DEBUG_SALLOC		
		printf("sfree:  invalid addr %8X\n", (unsigned int)addr);
#endif		
		return;
	}

	salloc_size -= LEN(addr);

#ifdef DEBUG_SALLOC		// Raymond 2008/03/06	
	salloc_cnt--;
	printf("sfree:  addr %8X - len = %6d / %d (%2d)\n", (unsigned int)addr, (unsigned int)LEN(addr), salloc_size, salloc_cnt);
#endif
    tr_del(addr);
    _sfree(addr);
}

static void *trim(void *addr, int len)
{
    int old_len = LEN(addr);
    unsigned *tail = (unsigned *)((char *)addr + len + LEN_BYTES);

    if (old_len < len + 64)
	return addr;
    tr_change(addr, len);
    LEN(addr) = len;
    LEN(tail) = old_len - len - LEN_BYTES;
    _sfree(tail);
    return addr;
}

void *srealloc(void *addr, int len)
{
    void *rc;
    unsigned **p;

#ifdef DEBUG_SALLOC
	printf("srealloc:  addr %8X - len = %6d -> %6d\n", (unsigned int)addr, (unsigned int)LEN(addr), len);
#endif

    len = roundup(len);

	// Raymond 2009/06/10
	salloc_size -= LEN(addr);
	salloc_size += len;
		
    if (LEN(addr) > len)
	return trim(addr, len);

    /* find right neighbour */
    for (p = &free_list; *p && !is_next_to(addr, *p); p = (void *)*p);

    if (*p && LEN(*p) + LEN(addr) + LEN_BYTES >= len)
    {
	unsigned *next = *p;
	*p = (void *)*next;
	*next = NULL;
	rc = join(addr, next);
	return trim(rc, len);
    }

    /* no luck -- just free and re-allocate */
    rc = salloc(len);
    memcpy(rc, addr, LEN(addr));
    sfree(addr);
    return rc;
}

