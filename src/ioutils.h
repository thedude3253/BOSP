#include<stdint.h>
unsigned char inb(unsigned short port); //returns the byte located at IO port
void outb(unsigned short port, unsigned char byte); //Attempts to write the given byte to the given IO port
unsigned char peek(intptr_t address);   //Returns the byte located at the address given
void poke(intptr_t address, unsigned char byte);    //sets the memory address to the byte given