#include "../reg.h"
#include "syscall.h"
#include "mem.h"
#include "string.h"

void generic_syscall(char code, const char* in, int in_size) {
    if (in != 0) memcpy(SYSCALL_PAGE + 1, in, in_size);
    SYSCALL_PAGE[0] = code;
    NMI_TRIGGER = 0;

    SYSCALL_PAGE[0] = 0;
}

void print(char* str) {
    generic_syscall(0x01, str, strlen(str) + 1);
}

const char* hex_digits = "0123456789abcdef";

void print_hex(int value) {
    char str[5];
    
    str[0] = hex_digits[(value >> 12) & 0xf];
    str[1] = hex_digits[(value >> 8) & 0xf];
    str[2] = hex_digits[(value >> 4) & 0xf];
    str[3] = hex_digits[value & 0xf];
    str[4] = 0;

    print(str);
}

void scan(char* buf) {
    generic_syscall(0x02, 0, 0);
    strcpy(buf, SYSCALL_PAGE + 1);
}

void spawn(void* func) {
    generic_syscall(0x03, (char*) &func, 2);
}

void exit() {
    generic_syscall(0x04, 0, 0);
}