bits 16
org 0x7c00
mov [BOOT_DISK],dl
bootstart:
    call initscreen
	call printStamp
	call readMemory
    jmp 0x8000

headerString: db 'BaseDOS Project Attempt 3',0x00
versionNumberString: db 'Booting from disk...',0x00
bootFailString: db 'Failed to read kernel from disk!',0x00
haltString: db 'System halted.',0x00
BOOT_DISK: db 0x00

initscreen:
    ;first we set display mode to 80x25 text (clears display)
    mov ax,0x0003
    int 0x10
	;then we set the cursor position to the top left
    mov bh,0x00
    mov dx,0x0000
    mov ah,0x02
    int 0x10
	;then we set the background to blue cause its a nice color uwu
	mov ah,0x0b
	mov bx,0x0001
	int 0x10
	;now we can go back to whatever we were doing
    ret

print:	;prints a null terminated string stored at address pointed to by bx, if dl != 0 it ends with a nlcr else does nothing
	push dx
	mov cx,0x0000
	mov ah,0x0e
	.printloop:
		mov dx,[bx]
		cmp dl,0
		je .return
		mov al,dl
		int 0x10
		inc bx
		jmp .printloop
	.return:
	pop dx
	cmp dl,0
	je .printret
	mov al, 0x0a
	int 0x10
	mov al,0x0d
	int 0x10
	.printret:
	ret

printStamp:	;prints the name and version of the OS
	mov bx,headerString
	mov dl,0x01
	call print

	mov bx,versionNumberString
	call print
	ret

halt:	;puts the cpu to rest
	mov bx,haltString
	call print
	.hloop:
	hlt
	jmp .hloop

readMemory:	;Reads the rest of the bootloader into memory
	mov ax,0x0232	;ah 02h is read from disk for int 13, al is how many sectors to read
	mov cx,0x0002	;ch is cylinder number, cl is sector number.
	mov dh,0x00	;Head number
	mov dl,[BOOT_DISK]	;drive number
	mov bx,0x8000	;where to write the data to in RAM (In this case, right after the boot sector)
	int 0x13	;Disk interrupt
	jc .diskReadError	;If the interrupt fails for any reason it sets the carry flag, so we can use a conditional jump to let the user know
	ret
	.diskReadError:
		mov bx,bootFailString
		mov dl,0x01
		call print
		jmp halt

times 510 - ($-$$) db 0
dw 0xaa55