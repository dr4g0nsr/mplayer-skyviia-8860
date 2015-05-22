#include <sys/syscall.h>
#include "new_syscalls_temp.h"
#include <errno.h>
#if defined(CONFIG_USE_KERNEL_2_6_32)
#define __NR_svread 365+0x900000 
#elif defined(CONFIG_USE_KERNEL_2_6_27)
#define __NR_svread 361+0x900000 
#endif

_syscall3(unsigned int, svread, unsigned int, id, void *, buf, unsigned int, count);


int __set_errno(int n)
{
    errno = n;
    return -1;
}

/*
 * this function is called from syscall stubs,
 * (tail-called in the case of 0-4 arg versions)
 */

int __set_syscall_errno(int n)
{
        /* some syscalls, mmap() for example, have valid return
        ** values that are "negative".  Since errno values are not
        ** greater than 131 on Linux, we will just consider 
        ** anything significantly out of range as not-an-error
        */
    if(n > -256) {
        return __set_errno(-n);
    } else {
        return n;
    }
}
