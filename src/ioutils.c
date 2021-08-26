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
extern uint8_t charBufferPointer;
extern uint8_t charLimit;
extern uint8_t charBuffer[];
unsigned char getKey() {
    uint8_t ret;
    if(charLimit == charBufferPointer) {
        ret = 0x00;
    }
    else {
        ret = charBuffer[charBufferPointer];
        charBufferPointer++;
    }
    return ret;
}