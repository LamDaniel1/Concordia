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
wild:         cmp byte [keith] , 0
horses:       je flash
brown:        cmp byte [mick]  , 0
sugar:        je flash
dead:         mov bl , [mick]
tattoo_you:   add al , [keith] 
flowers:      sub bl, 1       
gimme:        cmp bl, 0      
shelter:      jnz tattoo_you   
flash:        mov [stones], ax 
  
  
              mov eax,1            ; The system call for exit (sys_exit)
              mov ebx,0            ; Exit with return code of 0 (no error)
              int 80h

