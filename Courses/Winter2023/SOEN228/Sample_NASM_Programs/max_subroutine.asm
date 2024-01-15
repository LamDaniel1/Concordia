section .data

num1 db 1,2,3,4
num2 db 5,1,7,2

section .bss

; put UNINITIALIZED data here using

ans resb 4  
 
section .text


; define a subroutine which determines the larger
; of two arguments which are passed via the al and bl registers
; the subroutine returns the result in the dl register

max:        cmp al, bl      ; is al >= bl ?
            jae al_bigger  ; if true, then max is al
            mov dl, bl      ; else bl is the max so mov it into 'result' reg. dl
            ret             ; and return from the subroutine
al_bigger:  mov dl, al      ; al >= bl, so load dl with al and return 
            ret


        global _start

_start:
keith:   mov ecx , 4 ; initialize loop counter to 4
         mov edi, num1 ; load first address
         mov esi, num2 ; load second address
         mov ebp, ans  
again:   mov al, [edi] ; get first number from memory into "parameter" register
         mov bl, [esi] ; get second number from memory into "parameter" register
         call max      ; invoke the subroutine 
         mov [ebp] , dl ; save the max into memory
         inc edi
         inc esi
         inc ebp
         loop again
         
  
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

