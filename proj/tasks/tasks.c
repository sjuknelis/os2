#include "../reg.h"
#include "../io/input.h"
#include "../stdlib/mem.h"
#include "context.h"
#include "ksyscall.h"

char running_tasks[16], blocked_tasks[16];

void init_task(void* func) {
    char mb;
    for (mb = 1; mb < 16; mb++) {
        if (!running_tasks[mb]) break;
    }
    running_tasks[mb] = 1;

    INIT_TASK_VECTOR[0] = (int) func & 0xff;
    INIT_TASK_VECTOR[1] = ((int) func >> 8) & 0xff;

    KERNEL_SYSCALL_BLOCK[mb << 8] = 0;

    init_task_asm(mb);
}

char running_mb;

void run_tasks() {
    char any_task_run;
    while (1) {
        any_task_run = 0;
        for (running_mb = 1; running_mb < 16; running_mb++) {
            if (running_tasks[running_mb]) {
                handle_input();
                handle_syscall();

                if (!blocked_tasks[running_mb] && running_tasks[running_mb]) {
                    continue_task_asm(running_mb);
                    any_task_run = 1;
                }
            }
        }

        if (!any_task_run) {
            ZERO_ON_NMI = 1;
            IO_LOCKOUT = 1;
            while (ZERO_ON_NMI);
        }
    }
}

void set_blocked(char blocked) {
    blocked_tasks[running_mb] = blocked;
}

void kspawn(char* buf) {
    void* func_ptr = (void*) (buf[0] | (buf[1] << 8));
    init_task(func_ptr);
}

void kexit() {
    running_tasks[running_mb] = 0;
}