#include<stdint.h>
#include"ioutils.h"
#include"vgautils.h"
unsigned static char backgroundColor = 0x10;
unsigned static char foregroundColor = 0x0F;
unsigned static char cursorX = 0x00;
unsigned static char cursorY = 0x00;

void setColor(unsigned char back, unsigned char front) {
    backgroundColor = back;
    foregroundColor = front;
}

void moveCursor(int x, int y) {
    if(x<0) x=0;
    if(y<0) y=x=0;
    if(x>=80 && y>=24) {
        x = 79;
        y = 24;
    }
    if(y >= 25) y = 24;
    unsigned short pos = y * 80 + x;
    outb(0x03D4,0x0F);
    outb(0x03D5,(unsigned char)(pos & 0xFF));
    outb(0x03D4,0x0E);
    outb(0x03D5,(unsigned char)((pos>>8) & 0xFF));
    cursorX = x;
    cursorY = y;
}

void incCursor() {
    if(cursorX+1 >= 80) {
        moveCursor(0,cursorY+1);
    }
    else {
        moveCursor(cursorX+1,cursorY);
    }
}

void arrowHandler(uint8_t keyCode) {
    if(keyCode == 0xF1) {
        subCursor(1);
    }
    else if(keyCode == 0xF2) moveCursor(cursorX,cursorY-1);
    else if(keyCode == 0xF3) incCursor();
    else if(keyCode == 0xF4) moveCursor(cursorX,cursorY+1);
}

void printChar(unsigned char ascii) {
    if(ascii == '\n') {
        moveCursor(0,cursorY+1);
    }
    else if(ascii > 0xF0) {
        arrowHandler(ascii);
        return;
    }
    else if(ascii == '\t') {
        if(cursorX%4 == 0) {
            printChar(' ');
        }
        while(cursorX%4) {
            printChar(' ');
        }
    }
    else if(ascii == 0x08) {
        subCursor(1);
        printChar(' ');
        subCursor(1);
    }
    else {
    unsigned char cbyte = backgroundColor | foregroundColor;
    volatile uint16_t * position;
    position = (volatile uint16_t *)0xb8000 + (cursorY*80 + cursorX);
    *position = (cbyte << 8) | ascii;
    incCursor();
    }
}

void clearScreen() {
    volatile uint16_t * position;
    position = (volatile uint16_t *)0xb8000;
    unsigned char cbyte = backgroundColor | foregroundColor;
    while(position < (volatile uint16_t *)0xb8fa1) {
        *position = (cbyte << 8) | 0x20;
        //position = position + 2;
        position++;
    }
    moveCursor(0,0);
}

void print(char *s) {
    while(*s) {
        printChar(*s++);
    }
}

void println(char *s) {
    print(s);
    printChar('\n');
}

void printByte(uint8_t byte) {
    uint8_t tmp = (byte >> 4);
    if(tmp < 10) {
        printChar(tmp + 0x30);
    }
    else {
        printChar(tmp+0x37);
    }
    if((byte & 0x0F) < 10) {
        printChar((byte&0x0F) + 0x30);
    }
    else {
        printChar((byte&0x0F) + 0x37);
    }
}

void printHex(unsigned long h) {
    //split h into number of bytes
    int s = sizeof(h);
    for(int i = 0; i<s; ++i) {
        printByte((h >> 8*(s-1-i)) & 0xFF);
        printChar(' ');
        if((i+1)%8 == 0) {
            printChar(' ');
        }
    }
}

void subCursor(int value) {
    for(int i = 0; i<value; ++i) {
        if(cursorX-1 < 0) {
            moveCursor(79,cursorY-1);
        }
        else {
            moveCursor(cursorX-1,cursorY);
        }
    }
}

void printCharRaw(unsigned char c) {
    unsigned char cbyte = backgroundColor | foregroundColor;
    volatile uint16_t * position;
    position = (volatile uint16_t *)0xb8000 + (cursorY*80 + cursorX);
    *position = (cbyte << 8) | c;
    incCursor();
}
void printDebug(char *str, uint8_t x, uint8_t y) {
    uint8_t xstore = cursorX;
    uint8_t ystore = cursorY;
    char *temp = str;
    moveCursor(x,y);
    printChar('$');
    while(*str) {
        printCharRaw(*str++);
    }
    printChar('%');
    moveCursor(xstore,ystore);
}

int tabFunction() {
    int ret = 0;
    if(cursorX%4 == 0) {
        printChar(' ');
        ret++;
    }
    while(cursorX%4) {
        printChar(' ');
        ret++;
    }
    return ret;
}