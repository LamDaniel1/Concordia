section .data

; put your data in this section using
; db , dw, dd directions

loc1 db 5 
loc2 db 6

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
keith:  ; put your code here
  
        ; 

        mov  [loc1] , [loc2]  ; this will not assemble
  
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

