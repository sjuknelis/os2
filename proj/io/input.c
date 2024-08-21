#include "../reg.h"
#include "input.h"
#include "../stdlib/string.h"
#include "../tasks/tasks.h"

char input_read_index = 0;

char scan_buf[256];
char scan_index = 0, scan_ready = 0;

void handle_input() {
    if (scan_ready) return;

    while (input_read_index != INPUT_WRITE_INDEX) {
        char input_char = INPUT_PAGE[input_read_index++];

        if (input_char == ENTER) {
            IO_OUT = '\r';
            IO_OUT = '\n';
            scan_buf[scan_index] = 0;
            scan_index = 0;
            scan_ready = 1;
            break;
        } else if (input_char == BACKSPACE) {
            IO_OUT = BACKSPACE;
            scan_buf[--scan_index] = 0;
        } else {
            IO_OUT = input_char;
            scan_buf[scan_index++] = input_char;
        }
    }
}

void kscan(char* buf) {
    if (scan_ready) {
        strcpy(buf, scan_buf);
        scan_ready = 0;
        set_blocked(0);
    } else {
        buf[0] = 0;
        set_blocked(1);
    }
}