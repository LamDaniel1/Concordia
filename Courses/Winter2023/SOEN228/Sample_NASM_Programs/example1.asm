section .data

section .text
        global _start

_start:
        mov eax,4            ; 
keith:  mov ebx,1            ;  we add the label keith for debugging purposes
        add eax, ebx


        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h
