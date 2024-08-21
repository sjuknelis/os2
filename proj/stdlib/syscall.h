#ifndef SYSCALL_H
#define SYSCALL_H

void print(char* str);
void print_hex(int value);
void scan(char* buf);
void spawn(void* func);
void exit();

#endif