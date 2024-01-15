section .data

; put your data in this section using
; db , dw, dd directions

; examples of db (DEFINE BYTE assembler directive )

num1 db 5 ; decimal number 5
num2 db 0xa2 ; hexadecimal
num3 db 0a2h ; alternative form of specifying hexadecimal notation
num4 db $0a2 ; hex yet again, the leading 0 IS REQUIRED 
num5 db 10100010b ; same number specified in binary

; examples of dw (DEFINE WORD assembler directive)

num6 dw 321 ; decimal 321 as a WORD of memory = 2 BYTES
num7 dw 0x2ab6 ; how will this word be stored in memory?
               ; which byte is stored first?

num8 dw 0010101010110110b ; same number as above but in binary

; examples of dd (DEFINE DOUBLEWORD assembler directive)

num9 dd 0x12345678  ; 4 bytes of memory required to store a double word

; we can even define ASCII characters and ASCII character strings

letter db 'a'  ; ASCII code for 'a' will be stored in memory location letter
more_letters db 'b','c','d','e'  ; if more than one letter, separate with commas
keith1 db 'keith' ; a character string is enclosed in single quotes
mick db 'mick'  ; another string

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 


num10 resb 10 ; reserve 10 bytes
num11 resw 2 ; reserve 2 words
 
section .text
        global _start

_start:
keith:  ; put your code here
  
        ; 
  
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

