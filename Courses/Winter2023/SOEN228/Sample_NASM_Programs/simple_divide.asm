section .data


dividend db 5
divisor  db 2

section .bss

quotient resb 1
remainder resb 1
 
section .text
        global _start

_start:
keith:  cmp byte [divisor] , 0  ; compare divisor with 0
mick:   je exit                 ; if yes, then exit program
        mov al, [dividend]
        mov bl, 0   ; clear the quotient counter
again:  cmp al, [divisor]  ; is dividend >= divisor still
        jb done  ; if not we are finished 
        inc bl   ; increment the quotient counter and
        sub al, [divisor]  ; subract the divisor from current dividend
        jmp again
done:   mov [quotient] , bl
        mov [remainder], al
  
        ; 
  
exit:   mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

