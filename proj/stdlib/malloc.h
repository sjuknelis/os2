#ifndef MALLOC_H
#define MALLOC_H

#define MALLOC_START 0x3000
#define MALLOC_SIZE 0x1000
#define MALLOC_BLOCK_SIZE 64

void* malloc(int size);
void free(void* ptr);

#endif