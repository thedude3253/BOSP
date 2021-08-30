section .heap
global heap
global freeMemory
freeMemory: dw 0x1000   ;This is supposed to use a BIOS interrupt to determine how much heapspace we have, for now just hard coded
heap: