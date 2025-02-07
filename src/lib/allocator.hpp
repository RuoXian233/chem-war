// This file is written in pure C syntax

#ifdef __cplusplus
    #include <cstdio>
    #include <cstdlib>
    extern "C" {
        #include <memory.h>
    }
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <memory.h>
#endif


typedef enum {
    Allocated = 1,
    Avalible = 2,
    Occupied = 3,
    Deleted = 4
} BlockState;

typedef struct {
    BlockState state;
    unsigned id;
    unsigned size;
    void *address;
    unsigned refCount;
} Block;


#define UNPACK(block) block->address
#define AS(type, block) ((type) UNPACK(block))
#define SIZE(block) block->size
#define ID(block) block->id
#define STATE(block) block->state
#define INCREF(block) block->refCount++
#define DECREF(block) if (block->refCount > 0) { block->refCount--; }

static unsigned allocationCount = 0;
static unsigned blocksCount = 0;
static Block **allocatedBlocks = NULL;


void InitalizeAlloator() {
    allocatedBlocks = (Block **) malloc(sizeof(Block *));
    allocatedBlocks[0] = NULL;
    fprintf(stderr, "----- ALLOCATOR INITALIZED -----\n");
}

void LogAllocation(Block *b) {
    fprintf(stderr, "Memory allocation: id %u, size %u bytes, address %p [block location %p]\n", ID(b), SIZE(b), UNPACK(b), (void *) b);
}

void LogRelease(Block *b) {
    fprintf(stderr, "Memory deallocation: id %u [remaining %u block(s)]\n", ID(b), blocksCount - 1);
}

void LogLeak(Block *b) {
    fprintf(stderr, "Memory leak detected: id %u, size %u, address %p\n", ID(b), SIZE(b), UNPACK(b));
}

void IncRef(Block *b) {
    INCREF(b);
    fprintf(stderr, "Incremented refcount of block %u to %u\n", ID(b), b->refCount);
}

void DecRef(Block *b) {
    DECREF(b);
    fprintf(stderr, "Decremented refcount of block %u to %u\n", ID(b), b->refCount);
}


Block *NewBlock(unsigned size) {
    blocksCount++;
    allocationCount++;
    allocatedBlocks = (Block **) realloc((void *) allocatedBlocks, sizeof(Block *) * (blocksCount + 1));
    Block *b = (Block *) malloc(sizeof(Block));
    b->size = size;
    b->state = Allocated;
    b->id = allocationCount - 1;
    b->address = malloc(size);
    allocatedBlocks[b->id] = b;
    LogAllocation(b);

    // Default set refcount to 1
    b->refCount = 1;
    return b;
}

void ReleaseBlock(Block *b) {
    LogRelease(b);
    blocksCount--;
    free(b->address);
    b->state = Deleted;
    b->refCount = 0;
}

void Finalize(int op) {
    fprintf(stderr, "----- ALLOCATOR FINALIZING -----\n");
    if (blocksCount != 0) {
        fprintf(stderr, "Memory was probably leaked, remaining block(s): %u\n", blocksCount);
    }

    for (unsigned i = 0; i < allocationCount; i++) {
        if (allocatedBlocks[i]->state != Deleted) {
            LogLeak(allocatedBlocks[i]);
            if (op) {
                ReleaseBlock(allocatedBlocks[i]);
            }
        } 
    }
}

void Check(int op) {

}
