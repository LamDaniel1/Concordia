section .data

; put your data in this section using
; db , dw, dd directions

electron dd 9.11e-31
avogadro dd 6.02e23
sevenpointfive dd 7.5e00
neg_sevenpointfive dd -7.5e00
sevenpointfive_ext dq 7.5e00
point_one dd 1.0e-1
point_one_ext dq 1.0e-1
exact_zero dd 0.0e00
pi dd 3.1415926
smallest_normal dd 1.175494e-38    ;  1.0 x 2 e-126
f1   dd  1.40129846432481707092372958499999999999990e-45;
f2   dd  1.40129846432481707092372958499999999999995e-45;
f3   dd  1.4012980461120606e-45
mid  dd  1.4012983e-45;
last dd  1.40129846432481707092372958500000000000000e-45;
smallest_denormal dd 7.006492e-46   ; 2e-127 x 2e-23  = 2e-150
section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
        ; put your code here   THIS IS THE FIRST ASSEMBLY LANGUAGE INSTRUCTION
keith:  ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

