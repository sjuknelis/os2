#ifndef REG_H
#define REG_H

#define INPUT_PAGE ((char*) 0x7d00)
#define SYSCALL_PAGE ((char*) 0x7e00)

#define INPUT_WRITE_INDEX *((char*) 0x7f02)
#define ZERO_ON_NMI *((char*) 0x7f03)
#define INIT_TASK_VECTOR ((char*) 0x7f10)

#define IO_OUT *((char*) 0x8000)
#define NMI_TRIGGER *((char*) 0x80fe)
#define IO_LOCKOUT *((char*) 0x80ff)

#endif