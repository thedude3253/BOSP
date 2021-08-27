#include<stdint.h>
unsigned char inb(unsigned short port); //returns the byte located at IO port
void outb(unsigned short port, unsigned char byte); //Attempts to write the given byte to the given IO port
unsigned char peek(intptr_t address);   //Returns the byte located at the address given
void poke(intptr_t address, unsigned char byte);    //sets the memory address to the byte given
unsigned char getKey();    //returns an ascii or control code recieved by the keyboard. Returns 0x00 if buffer is empty.
void *malloc(unsigned long long size); //Returns a pointer to a chunk of memory of the size given (in bytes)
void free(void *mem);   //frees the memory assigned to that address
void initHeap();    //Bootstraps the heap