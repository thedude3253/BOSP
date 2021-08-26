    bits 64
cursorX: dw 0x0000
cursorY: dw 0x0000

backgroundColor: db 0x10
foregroundColor: db 0x0F
sendEndIRQ:
    mov dx,0x0020
    mov al,0x20
    out dx,al
    ;mov dx,0x00A0
    ;out dx,al
    ret
setCursorAsm:   ;bx - collumn (0-79) ax - row (0-24)
    mov [cursorX],bx
	mov [cursorY],ax
	mov dl, 80
	mul dl
	add bx, ax
.setCursorX:	;lets you just set the x of the cursor without messing with it's y position
	mov dx, 0x03D4
	mov al, 0x0F
	out dx, al
	inc dl
	mov al, bl
	out dx, al
	dec dl
	mov al, 0x0E
	out dx, al
	inc dl
	mov al, bh
	out dx, al
	ret

printCharAsm:	;al=ascii code to be written
	mov dl,[backgroundColor]
	mov bl,[foregroundColor]
	or bl,dl
	mov ah,bl
	push ax
	mov edi,0xb8000
	mov eax,0
	mov ax,[cursorX]
	sal eax,1
	mov ebx,edi
	add eax,ebx
	mov edi,eax
	mov eax,0
	mov ax,[cursorY]
	imul eax,160
	mov ebx,edi
	add eax,ebx
	mov edi,eax
	mov eax,0
	pop ax
	stosw
	call incCursorAsm
	ret
	
incCursorAsm:
	mov bx,[cursorX]
	inc bx
	mov ax,[cursorY]
	call setCursorAsm
	ret
	
printByteAsm:	;prints the hex value bl as ascii on screen
	xor ax,ax
	mov al,bl
	push ax
	AND al,11110000b
	SHR al,4
	cmp al,0x09
	jle num2Handle
	jmp let2Handle
	num2Handle:
		add al,0x30
		call printCharAsm
		jmp lowNibbleHandle
	let2Handle:
		add al,0x37
		call printCharAsm
		jmp lowNibbleHandle
	;low nibble handle
	lowNibbleHandle:
	pop ax
	AND al,00001111b
	cmp al,0x09
	jle numHandle
	jmp letHandle
	numHandle:
		add al,0x30
		call printCharAsm
		jmp byteprintend
	letHandle:
		add al,0x37
		call printCharAsm
		jmp byteprintend
	byteprintend:
		ret

print64:    ;prints out the 64 bytes of RBX as hex
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    push rbx
    call printByteAsm
    pop rbx
    shr rbx,8
    ret

_generalException:
    cli
    hlt ;stops executing instructions.

_generalInterrupt:
    cli ;Makes it so that we can't jump away from this
    hlt ;stops executing instructions.
%macro isr_err_stub 1
isr_stub_%+%1:
    mov al,0x40
    mov [backgroundColor],al
    mov bx,0
    mov ax,0
    call setCursorAsm
    mov bl,%1
    call printByteAsm
    call _generalException
    iretq
%endmacro
%macro isr_no_err_stub 1
isr_stub_%+%1:
    mov al,0xE0
    mov [backgroundColor],al
    mov bx,0
    mov ax,0
    call setCursorAsm
    mov bl,%1
    call printByteAsm
    call _generalInterrupt
    iretq
%endmacro
%macro isr_IRQ_stub 1
isr_stub_%+%1:
    mov bx,0
    mov ax,2
    call setCursorAsm
    mov bl,%1
    call printByteAsm
    call sendEndIRQ
    iretq
%endmacro
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_stub_8:
    mov al,0x40
    mov [backgroundColor],al
    mov bx,78
    mov ax,0
    call setCursorAsm
    mov bl,0x08
    call printByteAsm
    call _generalException
    iretq
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_stub_14:
    pop rbx
    call print64
    mov al,0x70
    mov [backgroundColor],al
    mov rbx,cr2
    call print64
    cli
    hlt
    iretq
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
isr_stub_32:
    call sendEndIRQ
    iretq
isr_stub_33:
    mov eax,0
    mov dx,0x60
    in al,dx
    mov [currChar],al
    call sendEndIRQ
    iretq
isr_IRQ_stub    34
isr_IRQ_stub    35
isr_IRQ_stub    36
isr_IRQ_stub    37
isr_IRQ_stub    38
isr_IRQ_stub    39
isr_IRQ_stub    40

global currChar
currChar: db 0x00

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    40 
    DQ isr_stub_%+i
%assign i i+1
%endrep