#include "../reg.h"
#include "output.h"

void kprint(char* str) {
    char i = 0;
    while (str[i] != 0) {
        IO_OUT = str[i];
        i++;
    }
}

extern const char* hex_digits;

void kprint_hex(char value) {
    IO_OUT = hex_digits[(value >> 4) & 0xf];
    IO_OUT = hex_digits[value & 0xf];
}