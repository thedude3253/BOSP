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
    //uint8_t *vmem =(uint8_t *) 0xB8000;
    //uint8_t trash = *vmem;
    //*vmem = trash;
    if(charBufferPointer == charLimit) return ret;
    uint8_t key = charBuffer[charBufferPointer];
    charBufferPointer = charBufferPointer + 1;
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