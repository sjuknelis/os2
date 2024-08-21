#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

extern void reset6502();
extern void exec6502(uint32_t cycles);
extern void step6502();
extern void nmi6502();

uint8_t mem[0x100000];
char memory_bank = 0;
char fire_nmi = 0;
char io_lockout = 0;

char last_char = 0;

void* input_thread(void *args) {
    while (1) {
        last_char = getchar();
        fire_nmi = 1;
    }
    return 0;
}

uint8_t read6502(uint16_t address) {
    if (address < 0x8000) return mem[address | (memory_bank << 16)];
    else if (address == 0x8001) return last_char;
    else if (address == 0x8002) return memory_bank;
    else return mem[address];
}

void write6502(uint16_t address, uint8_t value) {
    if (address < 0x8000) mem[address | (memory_bank << 16)] = value;
    else if (address == 0x8001) last_char = 0; //!
    else if (address == 0x80fe) fire_nmi = 1;
    else if (!io_lockout) {
        if (address == 0x8000) {
            if (value == 0x7f) printf("\b \b");
            else printf("%c", value);
        }
        else if (address == 0x8002) memory_bank = value & 0x0f;
        else if (address == 0x80ff) io_lockout = 1;
    }
}

int main() {
    system("/bin/stty raw");
    setbuf(stdout, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, input_thread, NULL); 

    FILE* fptr = fopen("./proj/proj.out", "rb");
    fread(mem + 0x9000, 0x7000, 1, fptr);
    fclose(fptr);

    int counter = 0;

    reset6502();
    while (1) {
        step6502();

        if (++counter >= 100000) {
            fire_nmi = 1;
            counter = 0;
        }

        if (fire_nmi) {
            fire_nmi = 0;
            if (io_lockout) {
                io_lockout = 0;
                nmi6502();
            }
        }
    }

    system("/bin/stty cooked");
    return 0;
}