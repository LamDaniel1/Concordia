; Ted Obuchowicz


section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 
; if you're curious bss stands for Block Started by Symbol

 
section .text
        global _start

_start:
        ; put your code here   THIS IS THE FIRST ASSEMBLY LANGUAGE INSTRUCTION
keith:  ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

