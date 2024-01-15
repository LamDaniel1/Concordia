section .data


keith db 3
mick  db 2

section .bss


stones resw 1
 
section .text
        global _start

_start:
start_me_up:  mov eax , 0
jack:         mov ebx , 0
              mov ecx , 0
              mov cl, [keith]
              mov bl , [mick]
tattoo_you:   add al , cl ; S1
              sub bl, 1        ; S2
              jnz tattoo_you   ; S5
              mov [stones], ax ; S5
  
  
              mov eax,1            ; The system call for exit (sys_exit)
              mov ebx,0            ; Exit with return code of 0 (no error)
              int 80h

