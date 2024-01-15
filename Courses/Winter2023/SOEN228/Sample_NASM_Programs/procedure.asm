section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text


example:  mov eax, 1234h
          mov ebx, 1234h
          ret

        global _start

_start:
keith:  ; put your code here
         call example
ron:     call example


  
        ; 
  
charlie:mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

