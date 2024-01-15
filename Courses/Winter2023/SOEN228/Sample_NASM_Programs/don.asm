

section .data

; put your data in this section using
; db , dw, dd directions

val1 db 0x05 ; 
val2 db 0x04 ; 
val3 db 0x03 ;  
val4 db 0x02 ; 
val5 db 0x01 ; 

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 
; if you're curious bss stands for Block Started by Symbol

 
section .text
        global _start

_start:
        mov ecx, val2 
line2:  mov bx, [ecx]   
line3:  add ecx, 0x01  
line4:  mov ax, [ecx]  
line5:  add ax, bx

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

