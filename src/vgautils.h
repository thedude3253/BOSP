#include<stdint.h>
void setColor(unsigned char back, unsigned char front); //Sets the internal variables for the foreground and background colors, in the format back = 0xX0 front = 0x0X
void moveCursor(int x, int y);  //moves the cursor to the x,y coordinates given, with 0,0 indicating the top left corner and expanding to the right and down
void incCursor();   //pushes the cursor one space over, wrapping around to the (0,y+1) if it reaches the edge
void printChar(unsigned char ascii);    //prints an ascii encoded character to the screen at the cursor position and then calls incCursor();
void clearScreen(); //Floods the screen with spaces using the background color given by setColor() and then calls moveCursor(0,0)
void print(char *s);    //prints a string to the screen starting at the cursor position
void println(char *s);  //calls print(char *s) followed by a printChar('\n')
void printByte(uint8_t byte);   //prints a byte to the screen, formatted as a hexidecimal number