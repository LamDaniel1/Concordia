; Ted Obuchowicz


section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

line  resb  80 ; assume less than 81 characters entered from keyboard

; define the addresses of the various io ports

datain   equ 10h ; keyboard data register
k_status equ 11h ; keyboard status register
                   ; 0 = no data inside datain reg.
                   ; 1 = data is available inside datain reg.

dataout  equ 20h  ; printer data register
p_status equ 21h  ; printer status register
                    ; 0 = printer not ready to accept new data yet
                    ;  1 = printer is ready for new data

; this is the pretend "MEMORY MAPPED VERSION"


; reads one line of characters from keyboard and send it
; to the printer character by character and also saves it
; in memory buffer calledc 'line'. 

 
section .text
        global _start

_start:  

keith:     mov eax, line     ; put address of line into eax
wait_key:  cmp  byte [k_status] , 0 
           jz wait_key      ; wait until a key is pressed
           mov bl , [datain] ; read it from keyboard and save inside bl
wait_print:cmp byte  [p_status], 0
           jz wait_print ; wait for printer to become ready
           mov [dataout], bl ; send the character to the printer
           mov [eax], bl     ; and also save it in memory buffer
           cmp bl, 0dh       ; did we read the carriage return char yet?
           jnz wait_key      ; if not go read another character
           mov  byte [dataout] , 0Ah ; if yes send lien feed to printer

           mov eax,1            ; The system call for exit (sys_exit)
           mov ebx,0            ; Exit with return code of 0 (no error)
           int 80h

