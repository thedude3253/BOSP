bits 16
extendedBootloader:
	;Now that we've pulled the rest of the bootloader up by bootstraps, our next goal is to get the computer ready for the kernel
	;Note to self: If this turns out to take up less space than expected, put it inside the boot sector for ~efficency~
	;First we need to enter Protected Mode
	cli	;disables interrupts for the time being
	call detectDrives	;figure out where things are
	;next we enable A20 line, which lets us access more memory
	in al,0x92	;xxxxxxxx
	or al,0x02	;xxxxxx1x flicks on the twos bit
	out 0x92,al	;writes that to the same port
	;then we need to load a Global Discriptor Table
	lgdt [gdt_descriptor]
	;then we set the PE bit in control register 0 to tell the computer we've activated protected mode
	mov eax,cr0
	or al,0x01
	mov cr0,eax
	jmp codeseg:start32	;then we make the jump to 32 bit commands. This is done by a far jump, which loads the necessary code into the code segment and executes it there
	jmp $

detectRAM:	;We need to detect how much memory we have access to, so we can set up a heap for applications to use.
	mov eax,0x0000e820
	ret

global floppyDrives
floppyDrives: db 0x00

detectDrives:
	;detect floppy drives
	mov eax,0
	mov al,0x10
	mov dx,0x70
	out dx,al
	mov dx,0x71
	in al,dx
	mov [floppyDrives],al
	ret

gdt_nulldesc:
	dd 0
	dd 0
gdt_codedesc:
	dw 0xFFFF	;limit low
	dw 0x0000	;base low
	db 0x00	;base mid
	db 10011010b	;access byte
	db 11001111b 	;flags and limit high
	db 0x00	;base high
gdt_datadesc:
	dw 0xFFFF	;limit low
	dw 0x0000	;base low
	db 0x00	;base mid
	db 10010010b	;access byte
	db 11001111b 	;flags and limit high
	db 0x00	;base high
	
gdt_end:

gdt_descriptor:
	gdt_size:
		dw gdt_end - gdt_nulldesc - 1
		dq gdt_nulldesc

codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

bits 32

start32:
	;sets the segment registers to dataseg, setting up where data is to be stored, which in our GDT overlaps with code, so be careful
	mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	call detectCPUID
	call detectLongmode
	call setupIndentityPaging
	call editGDT
	jmp codeseg:Start64Bit
	jmp $

detectCPUID:
	pushfd
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	xor eax,ecx
	jz noCPUID
	ret
	
detectLongmode:
	mov eax,0x80000001
	cpuid
	test edx, 1 << 29
	jz noLongmode
	ret
	
noCPUID:
	hlt	;no CPUID support
noLongmode:
	hlt	;no longmode support

PageTableEntry equ 0x1000

setupIndentityPaging:
	mov edi,PageTableEntry
	mov cr3,edi
	mov dword [edi],0x2003
	add edi,0x1000
	mov dword [edi],0x3003
	add edi,0x1000
	mov dword [edi],0x4003
	add edi, 0x1000
	mov ebx,0x00000003
	mov ecx, 512
	
	.setEntry:
		mov dword [edi],ebx
		add ebx, 0x1000
		add edi,8
		loop .setEntry
		
	mov eax,cr4
	or eax, 1 << 5
	mov cr4, eax
	
	mov ecx,0xc0000080	;set model specific register to read from
	rdmsr	;read model specific register into eax
	or eax, 1 << 8	;flip a bit
	wrmsr	;write it back
	
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax
	
	ret

editGDT:
	mov [gdt_codedesc + 6], byte 10101111b
	mov [gdt_datadesc + 6], byte 10101111b
	ret

bits 64
[extern _start]
Start64Bit:
	mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call clearScreen
	call _start
	call clearScreen
	jmp $

clearScreen:
	mov edi, 0xb8000
	mov rax, 0x5020502050205020	;magenta and black spaces
	mov ecx,500
	rep stosq
	ret