section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
        ; put your code here   THIS IS THE FIRST ASSEMBLY LANGUAGE INSTRUCTION
keith:  ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE
        mov eax, 0         ; clear the entire register.
ron:    mov al, 11111100b  ;  -4 in 8-bit  2's comp
        sar al, 1          ;  al = 11111110 = -2 in two's comp  = -4 /2 = -2

        mov al, 11111000b  ; al = -8 in two's comp.
        sal al, 1          ; al = -16   in two's comp = -8 x 2 = -16

;       do the same but for some positive numbers

        mov eax, 0  ; clear the entire register
        mov al,   00001000b  ;   +8 in 8-bit two's comp
        sar al, 1            ; al = 00000100 = +4 = +8 / 2 

        mov al,  00001000b   ; +8 in 8-bit two's comp
        sal al, 1            ; al = 00010000  = +16 = +8 x 2 

;     now contrast these "arithmetic" shifts with the "regular shifts"
   
        mov eax, 0
        mov al, 11111100b
        shr al, 1    ; al = 01111110

        mov al, 11111100b
        shl al, 1    ; al = 11111000b 

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

