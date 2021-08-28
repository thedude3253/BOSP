#include"ioutils.h"
void sendEOI(unsigned char irqNum) {
    if(irqNum >= 8) outb(0xA0,0x20);
    outb(0x20,0x20);
}
void initPIC(int offset1, int offset2) {    //note, I really should be using a wait function between these outs, yet here we are
    unsigned char a1,a2;
    a1 = inb(0x21); //grabs the master mask
    a2 = inb(0xA1); //grabs the slave mask
    outb(0x20,0x10 | 0x01); //Tells the PIC we want to initialize it
    outb(0xA0,0x10 | 0x01);
    outb(0x21,offset1); //master PIC offset
    outb(0xA1,offset2); //slave PIC offset
    outb(0x21,0x04);    //Tell master PIC there is a slave PIC at IRQ2
    outb(0xA1,0x02);    //Tell slave PIC its "cascade identity"
    outb(0x21,0x01);    //Puts the master PIC into 8086 mode
    outb(0xA1,0x01);    //puts the slave PIC into 8086 mode
    outb(0x21,a1);  //restore the masks
    outb(0xA1,a2);
}