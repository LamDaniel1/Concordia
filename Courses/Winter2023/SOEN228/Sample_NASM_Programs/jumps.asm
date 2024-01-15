; Ted Obuchowicz


section .data

; put your data in this section using
; db , dw, dd directions


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

; jumps based upon testing a single flag in the status register

; jc   jump if carry
; jnc  jump if no carry
; jo   jump if overflow
; jno  jump if no overflow
; jp    jump if parity
; jnp  jump if no parity
; jpe  jump if parity even
; jpo  jump if parity odd
; js   jump if sign flag set
; jns  jump if sign flag clear
; jz   jump if zero
; jnz  jump if not zero


; jumps based after cmp first, second
;   result of compare      signed operands          unsigned operands
;     =                       je                       je;  
;     <>                      jne                      jne
;     >                       jg                       ja 
;     <                       jl                       jb
;    >=                       jge                      jae
;    <=                       jle                      jbe


        mov al, 11111111b
        mov bl, 11111110b
;  the following code performs
;  if ( al > bl)
;  {
      mov cl, 1
;  }
;  else
;  {
;     mov  cl,0
;  }
;  
;  mov dl, 5



ron:    cmp al, bl 
jack:   jg  al_bigger
bl_bigger:  mov cl , 0
        jmp next
al_bigger: mov cl, 1
next:   mov dl, 5 


        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

