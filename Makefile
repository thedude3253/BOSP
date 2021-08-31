CC = /usr/local/x86_64elfgcc/bin/x86_64-elf-gcc
LD = /usr/local/x86_64elfgcc/bin/x86_64-elf-ld
SRC := $(wildcard src/*.c)
TMPVAR := $(wildcard src/*.asm)
SRCA := $(filter-out src/bootloader.asm src/extendedloader.asm,$(TMPVAR))
OBJA := $(patsubst src/%.asm,out/%.o,$(SRCA))
OBJS := $(patsubst src/%.c,out/%.o,$(SRC))
CFLAGS = -ffreestanding -mno-red-zone -m64

out/file.img : out/kernel.bin out/bootloader.bin
	cat out/bootloader.bin out/kernel.bin > out/file.img

out/kernel.bin : $(OBJS) $(OBJA) out/loader.o
	$(LD) -Ttext 0x8000 -T"link.ld" out/loader.o $(OBJA) $(OBJS)

out/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o : src/%.asm
	@echo 
	nasm $< -f elf64 -o $@

out/bootloader.bin : src/bootloader.asm
	nasm src/bootloader.asm -f bin -o out/bootloader.bin

out/loader.o : src/extendedloader.asm
	nasm src/extendedloader.asm -f elf64 -o out/loader.o