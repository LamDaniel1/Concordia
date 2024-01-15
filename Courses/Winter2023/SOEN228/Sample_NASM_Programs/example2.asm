section .data

number: dd 1,2,3,4,5
sum:    dd  0          ; reserve 1 byte initialize to 0

 
section .text
        global _start

_start:
keith:  mov ecx, 5  ; initialize loop counter to 5 number in the list
ron:    mov eax, 0 ; clear the 'sum' register
        mov ebx, number     ; load ebx wth address of first number
again:  add eax,  [ebx]      ; add number from memory to 'sum' register
        add ebx,4           ; make ebx point to 'next' number in the array
        loop again
        mov [sum], eax        ; save answer back in memory location called sum
    
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h
