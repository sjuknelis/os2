#include "stdlib/malloc.h"
#include "stdlib/string.h"
#include "stdlib/syscall.h"
#include "tasks/tasks.h"

void printer() {
    long i = 0;
    int count = 0;
    while (count++ < 5) {
        print("b");
        for (i = 0; i < 100000; i++);
    }
    exit();
}

void echo() {
    char buf[64];
    while (1) {
        scan(buf);
        print(buf);
        print(" bob ");
        if (strcmp(buf, "print") == 0) {
            print("print!!!\r\n");
            spawn(printer);
        } else {
            print(itoa(strlen(buf), buf));
            print("\r\n");
        }
    }
}

void allocer() {
    char *ptr1, *ptr2, *ptr3, *ptr4;
    // while (1);
    ptr1 = malloc(256);
    print_hex((int) ptr1);
    print(" ");
    ptr2 = malloc(128);
    print_hex((int) ptr2);
    print(" ");
    free(ptr1);
    ptr3 = malloc(512);
    print_hex((int) ptr3);
    print(" ");
    free(ptr2);
    ptr4 = malloc(384);
    print_hex((int) ptr4);
    exit();
}

void main() {
    init_task(echo);
    // init_task(allocer);
    run_tasks();
}