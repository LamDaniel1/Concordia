; Ted Obuchowicz
; a simple program to determine
; whether a program can explicitly
; modify the EIP register
; my intuition tells me NO..
; lets see what NASM says

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

        mov eax, 5            ; this is allowed as eas is a
                              ; GENERAL PURPOSE register visible to the
                              ; assembly language programmer 

       mov eip, 0             ; this better not assemble
                              ; or else it will cause a whole lotta 
                              ; problems with the fetch-execute of this program
                              ; I'll give $10 ( yes 10 dollars) to the first person
                              ; who replies by email to me with the correct answer
                              ; as to why it will cause a whole lotta trouble if
                              ; this line we to assemble

                              ; OK.. good NASM vomited on this line:
                              ; can_you_modify_eip.asm:33: error: symbol `eip' undefined

                              ; someone replied eip can be a source register only
                              ; not a destination...I say that is still incorrect
                              ; and will still cause assemble time error if we try

       mov eax, eip            ; yep .. this still causes an error       
                              ; can_you_modify_eip.asm:48: error: symbol `eip' undefined 
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

