section .data

LIST1 db 1,2,3,4,5
LIST2 db 0,8,6,2,10
SIZE  db 5

section .bss

SMALLER resb 5
 
section .text
        global _start

_start:
        ; put your code here   THIS IS THE FIRST ASSEMBLY LANGUAGE INSTRUCTION
keith:  ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE
ron:    mov ecx, [SIZE]  ; get size of the lists
        mov esi, LIST1
        mov edi, LIST2
        mov edx, SMALLER ; initialize the pointers to each list
again:  mov al, [esi]
        mov bl, [edi]
        cmp al, bl
        jbe bl_les
        mov [edx], al
        jmp cont
bl_les: mov [edx], bl
cont:   inc esi
        inc edi
        inc edx
        loop again


        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

