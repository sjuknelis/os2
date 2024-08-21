#ifndef KSYSCALL_H
#define KSYSCALL_H

#define KERNEL_SYSCALL_BLOCK ((char*) 0x6000)

void handle_syscall();

#endif