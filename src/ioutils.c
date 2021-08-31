#include<stdint.h>
#include"ioutils.h"
unsigned char inb(unsigned short port) {
    unsigned char returnval;
    asm volatile ("inb %1,%0" : "=a" (returnval) : "d" (port));
    return returnval;
}

void outb(unsigned short port, unsigned char byte) {
    asm volatile ("out %0,%1" : : "a"(byte) , "d" (port));
}

uint8_t peek(intptr_t address) {
    uint8_t *byte;
    byte = (uint8_t *)address;
    return *byte;
}

void poke(intptr_t address, unsigned char byte) {
    uint8_t *byte2;
    byte2 = (uint8_t *)address;
    *byte2 = byte;
}
extern uint8_t volatile charBufferPointer;
extern uint8_t volatile charLimit;
extern uint8_t charBuffer[];
extern uint8_t keyMap[];
unsigned char fakeBool = 0;
unsigned char getKey() {
    unsigned char volatile ret;
    ret = 0x00;
    if(charBufferPointer == charLimit) return ret;
    uint8_t key = charBuffer[charBufferPointer];
    charBufferPointer = charBufferPointer + 1;
    if(key == 0xE0) {
        uint8_t key2 = charBuffer[charBufferPointer];
        charBufferPointer = charBufferPointer + 1;
        if(key2 == 0x4B) return 0xF1;
        if(key2 == 0x48) return 0xF2;
        if(key2 == 0x4D) return 0xF3;
        if(key2 == 0x50) return 0xF4;
    }
    if(key == 0xAA) {
        fakeBool = 0;
        return 0x00;
    }
    if(key < 0x3A && key > 0) {
        ret = keyMap[key];
    }
    if(ret == 0) {
        if(key == 0x2A) {
            fakeBool = 1;
            return 0x00;
        }
    }
    if(fakeBool > 0) {
        if(ret > 0x60 && ret < 0x7B) {
            ret = ret - 0x20;
        }
    }
    return ret;
}
extern uint8_t heap;
extern uint16_t freeMemory;

typedef struct memoryBlock {
    uint8_t used;
    uint8_t *startAddress;
} memoryBlock;

typedef struct mapPointer {
    uint8_t used;
    void *startAddress;
    uint8_t runLength;
} mapPointer;

memoryBlock heapMap[256];
mapPointer mapMap[256];

void initHeap() {
    uint16_t freeBlocks = freeMemory / 0x10;
    uint8_t *heapStart = &heap;
    for(int i = 0; i<freeBlocks; ++i) {
        memoryBlock *block = &heapMap[i];
        block->used = 0;
        block->startAddress = heapStart + (i*0x10);
    }
}

void *malloc(unsigned long long size) {
    int numblocks = size/0x10;
    if(numblocks == 0) numblocks++;
    int fakeBool = 0;
    memoryBlock *block;
    void *ret = 0;
    for(int i = 0; i<256; ++i) {
        if(!heapMap[i].used && !fakeBool) {
            block = &heapMap[i];
            ret = block->startAddress;
            fakeBool++;
        }
        else if(!heapMap[i].used) {
            fakeBool++;
            if(fakeBool >= numblocks) {
                int j = 0;
                while(mapMap[j].used) {
                    ++j;
                }
                mapPointer pointer = mapMap[j];
                pointer.used = 1;
                pointer.runLength = numblocks;
                pointer.startAddress = ret;
                return ret;
            }
        }
        else {
            fakeBool = 0;
            ret = 0;
        }
    }
}

void free(void *mem) {
    int j = 0;
    int h = 0;
    while(mapMap[j].startAddress != mem) {
        ++j;
        if(j>255) return;
    }
    while(heapMap[h].startAddress != mem) {
        ++h;
        if(h>255) return;
    }
    for(int i = 0; i<mapMap[j].runLength; ++i) {
        heapMap[h].used = 0;
        ++h;
    }
    mapMap[j].used = 0;
}

void *getBlock() {
    for(int i = 0; i<256; ++i) {
        if(heapMap[i].used == 0) {
            heapMap[i].used = 1;
            return heapMap[i].startAddress;
        }
    }
    return 0;
}

void freeBlock(void *mem) {
    for(int i = 0; i<256; ++i) {
        if(heapMap[i].startAddress == mem) {
            heapMap[i].used = 0;
            return;
        }
    }
}

void readDisk(int drive, uint32_t offset, void *destination) {
    //TODO: write code
}

void writeDisk(int drive, uint32_t offset, void *source) {
    //TODO: write code
}