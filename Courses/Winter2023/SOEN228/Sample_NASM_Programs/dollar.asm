section .data

; put your data in this section using
; db , dw, dd directions

msg0 db "blah"
message1 db "hello"
length equ $-message1 ; length = 5
length2 equ $-msg0    ; length2 = 5+4 = 9

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
keith:  ; put your code here
  
        ; 
        mov eax, length
        mov ebx, length2
  
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

