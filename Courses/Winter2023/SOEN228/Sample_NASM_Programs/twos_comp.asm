; Ted Obuchowicz


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
        ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE

ron:    mov eax, 0
keith:  mov al, 00000011b    ;  al = +3 in 8-bit 2's comp notation

; now we change the +3 into -3 by taking the 2's comp

        not al ;   negate the bits in all 1's becomes 0's and 0s turn into 1s
               ; al = 1111 1100 = FC
        inc al ;   add 1 to al can be done as add al, 1
               ; al = 1111 1101 = -3  = FD

; the same can be done with the NEG reg instruction which negates the value
; stored in a register by taking it's two's complement 

        mov al , 00000011b  ; al  = 03
        neg al  ; al = 1111 1101 = FD

 
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

