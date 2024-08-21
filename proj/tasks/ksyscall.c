#include "../reg.h"
#include "ksyscall.h"
#include "../io/input.h"
#include "../io/output.h"
#include "../stdlib/syscall.h"
#include "../tasks/tasks.h"

void (*syscall_handlers[])(char*) = {
    0,
    kprint,
    kscan,
    kspawn,
    kexit
};

extern char running_mb;

void handle_syscall() {
    char* syscall_page = KERNEL_SYSCALL_BLOCK + (running_mb << 8);
    if (syscall_page[0] != 0) syscall_handlers[syscall_page[0]](syscall_page + 1);
}