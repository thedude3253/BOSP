#include"vgautils.h"
#include"picutils.h"
#include"ioutils.h"
#include"extrautils.h"
typedef struct idtr {
    uint16_t limit; //Defines the length of the idt in bytes minus one. (minimum value is 0x100)
    uint64_t base;  //The linear address where the idt starts (INT 0)
} __attribute__((packed)) idtr;
typedef struct idt_entry {
    uint16_t offset_low;    //Offset low (0 - 15)
    uint16_t selector;  //Code segment selector
    uint8_t zero;   //zeros
    uint8_t attributes; //type and attributes
    uint16_t offset_middle; //Offset middle bits (16-31)
    uint32_t offset_high;   //Offset high bits (32 - 63)
    uint32_t more_zero; //More zeros
} __attribute__((packed)) idt_entry;
idt_entry IDT[256];
idtr IDTR;
extern void *isr_stub_table[];
void set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry *descriptor = &IDT[vector];
    descriptor->offset_low = (uint64_t)isr & 0xFFFF;    //takes the lowest 16 bits of the address
    descriptor->selector = 0x08;    //GDT code offset
    descriptor->zero = 0x00;    //zeros
    descriptor->attributes = flags;
    descriptor->offset_middle = ((uint64_t)isr >> 16) & 0xFFFF; //8888888877777777666666665555555544444444333333332222222211111111 -> 0000000000000000888888887777777766666666555555554444444433333333 -> 0..04444444433333333
    descriptor->offset_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;   //high 32 bits of isr address
    descriptor->more_zero = 0x00000000;
}
void initIDT() {
    IDTR.base = (uint64_t)&IDT[0];
    IDTR.limit = (uint16_t)sizeof(idt_entry) * 39;
    for(uint8_t vector = 0; vector < 40; vector++) {
        set_descriptor(vector,isr_stub_table[vector],0x8E);
    }
    __asm__ volatile ("lidt %0" : : "memory"(IDTR)); // load the new IDT
    initPIC(0x20,0x28);
    __asm__ volatile ("sti"); // set the interrupt flag
}
uint8_t mode = 0x01;
unsigned char command[256];
unsigned char pointer = 0x00;
void clearCommandBuffer() {
    for(int i = 0; i<255; i++) {
        command[i] = 0;
    }
    pointer = 0;
}
void parseCommand(char *s) {
    if(stringEqu("text",s)) {
        mode = 0;
        clearCommandBuffer();
        clearScreen();
    }
    else if(stringEqu("help",s)) {
        print("\nCommands:\nhelp - what you're running right now\ntext - activates the text editor/sandbox mode\njoke - says a funny");
        clearCommandBuffer();
    }
    else if(stringEqu("joke",s)) {
        print("\nbogos binted?");
        clearCommandBuffer();
    }
    else {
        printChar('\n');
        print(s);
    }
}

void _start() {
    setColor(0x10,0x0F);
    clearScreen();
    println("BaseDOS Project Version 0.3");
    println("Designed by Jason Rowe");
    initIDT();
    if(mode == 0x01) print("\n>");
    while(1) {
        unsigned char key = getKey();
        if(key > 0) {
            if(mode == 0x01) {
                if(key == '\n') {
                    char *temp = &command[1];
                    parseCommand(temp);
                    for(int i = 0; i<255; i++) {
                        command[i] = 0;
                    }
                    pointer = 0;
                    if(mode == 0x01)print("\n>");
                }
                else if(key == 0x08) {
                    if(pointer > 0) {
                        printChar(0x08);
                        command[pointer] = 0;
                        pointer--;
                    }
                }
                else if(key == '\t') {
                    int tempInt = tabFunction();
                    while(tempInt) {
                        pointer++;
                        command[pointer] = ' ';
                        tempInt--;
                    }
                }
                else if(key > 0xF0) {
                    if(key == 0xF1 && pointer > 0) {
                        printChar(key);
                        pointer--;
                    }
                    if(key == 0xF3 && (command[pointer+1] != 0)) {
                        printChar(key);
                        pointer++;
                    } 
                }
                else {
                    printChar(key);
                    pointer++;
                    command[pointer] = key;
                }
            }
            else if(key == 0x1B) {
                mode = 1;
                clearScreen();
                print("\n>");
            }
            else printChar(key);
        }
    }
    return;
}