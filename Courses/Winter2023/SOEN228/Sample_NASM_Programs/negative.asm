section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
keith:  ; put your code here
  
        ; 
        mul bx, -1 ; does this even assemble? 


; no it does not
;ted@deadflowers Code 4:37pm >nasm -f elf negative.asm -l negative.lis
;negative.asm:20: error: invalid combination of opcode and operands

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

