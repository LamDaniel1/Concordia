section .data

; put your date in this section using 
; db, dw, dd directives

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 )
buffer resb 5   ; reserve 5 bytes of memory 


section .text
        global _start

_start:

;       first read in up to 5 characters from keyboard
        mov eax,3            ; the system call for read
keith:  mov ebx,2            ; File descriptor 2 - standard input
        mov ecx,buffer       ; put the offset of the buffer in ecx
        mov edx,5            ; maximum number of chars to read
        int 80h
       
        mov eax,4            ; The system call for write (sys_write)
        mov ebx,1            ; File descriptor 1 - standard output
        mov ecx,buffer       ; Put the offset of buffer in ecx
        mov edx,5            ; lenght of string
        int 80h              ; Call the kernel

        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h
