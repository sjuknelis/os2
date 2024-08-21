#include "malloc.h"
#include "mem.h"
#include "syscall.h"

#define BLOCK_COUNT (MALLOC_SIZE / MALLOC_BLOCK_SIZE)
char blocks_used[BLOCK_COUNT];

int find_free_index(int blocks) {
    int i, free_found = 0;

    for (i = 0; i < BLOCK_COUNT; i++) {
        if (!blocks_used[i]) {
            if (++free_found >= blocks) {
                return i - blocks + 1;
            }
        } else {
            free_found = 0;
        }
    }

    return -1;
}

void* malloc(int size) {
    int blocks = (size - 1) / MALLOC_BLOCK_SIZE + 1, i;

    int free_index = find_free_index(blocks);
    if (free_index == -1) return 0;

    blocks_used[free_index] = blocks;
    for (i = 1; i < blocks; i++) blocks_used[free_index + i] = 1;

    return (void*) (MALLOC_START + free_index * MALLOC_BLOCK_SIZE);
}

void free(void* ptr) {
    int block_index, blocks, i;
    if (ptr < (void*) MALLOC_START || ptr >= (void*) (MALLOC_START + MALLOC_SIZE)) return;

    block_index = ((int) ptr - MALLOC_START) / MALLOC_BLOCK_SIZE;
    blocks = blocks_used[block_index];

    for (i = 0; i < blocks; i++) blocks_used[block_index + i] = 0;
}