section .data

; put your data in this section using
; db , dw, dd directions


list1 db 1,2,3,4,5
list2 db 6,7,8,9,10

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 


list3 resb 5 
 
section .text
        global _start

_start:
  ; put your code here
keith:    mov ecx , 6 ; initialize loop counter
ron:      mov esi, 0  ; clear index registers
          mov edi, 0  
          mov edx, 0
again:    mov al , [esi + list1]
          mov bl , [edi + list2] ;
          add bl, al ;
          mov [list3+ edx] , bl
          inc esi ; increment the 3 pointers
          inc edi
          inc edx
          loop again
           

        ; 
  
mov eax,1            ; The system call for exit (sys_exit)
mov ebx,0            ; Exit with return code of 0 (no error)
int 80h

