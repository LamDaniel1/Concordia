section .data

; put your data in this section using
; db , dw, dd directions

mick dw 30000
keith dw 0ABCDH
charlie dw 7FFFh   ; this is 32767 which is > 30000
ron dw 1000100010001000B   ; this is the smallest  
                           ; (assuming signed operands)


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
        ; put your code here   THIS IS THE FIRST ASSEMBLY LANGUAGE INSTRUCTION
keith1:  ; THIS IS THE SECOND ASSEMBLY LANGUAGE INSTRUCTION  
        ; 
        ; THE REST OF THE PROGRAM IS ENTERED HERE

        mov eax,0
here:   mov edx,0            ; clear the two registers
        mov ecx,3            ; initialize loop counter
        mov esi, mick        ; load address into esi
        mov ax, [esi]       ; assume m[mick] is largest
        add esi,2           ; point the next number after mick
again:  cmp [esi],ax
        jl cont             ; if m[esi] < ax continue the loop, else
        mov ax,[esi]        ; found a new bigger so put it in ax
cont:   add esi,2           ; point to next number in memory 2 bytes away
        loop again
        
        ; now do the same except look for the smallest

        mov ecx, 3
        mov esi,mick
        mov dx,[esi]       ; assume first number is the smallest
        add esi,2
again2: cmp [esi],dx
        jg cont2           ; if m[esi] > dx continue the loop, else 
        mov dx,[esi]       ; we found a new smallest so put it inside dx
cont2:  add esi,2
        loop again2        

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h



; for unsigned operands, the above code is the same except
; that we would use ja (instead of jg) and jb (instead of jl)


