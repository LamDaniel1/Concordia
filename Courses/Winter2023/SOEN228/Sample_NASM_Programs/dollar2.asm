section .data

; put your data in this section using
; db , dw, dd directions


name1 db   'Keith Richards'
how_long equ $ - name1
name2 db    'Mick Jagger'
how_long_is_mick equ $ - name2
song db 'Brown Sugar'
letters_in_song_title equ $ - song

section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

 
section .text
        global _start

_start:
keith:  ; put your code here
  
        ; 
        mov eax,  how_long
        mov ebx,  how_long_is_mick
        mov ecx, letters_in_song_title
  
        mov eax,1            ; The system call for exit (sys_exit)
        mov ebx,0            ; Exit with return code of 0 (no error)
        int 80h

