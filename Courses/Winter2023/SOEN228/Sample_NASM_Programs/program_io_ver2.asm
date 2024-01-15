; Ted Obuchowicz


section .data

; put your data in this section using
; db , dw, dd directions


section .bss

; put UNINITIALIZED data here using
; sum resb 5 format (read as sum reserve bytes 5 

line  resb  80 ; assume less than 81 characters entered from keyboard

; define the addresses of the various io ports
; uses the IO instructions 
; IN accumulator, port#
; OUT port#, accumulator
; the accumulator is the ax register (or the al part if working with byte wide port)
; 
; this is the pretend "IO-MAPPED VERSION"


datain   equ 10h ; keyboard data register
k_status equ 11h ; keyboard status register
                   ; 0 = no data inside datain reg.
                   ; 1 = data is available inside datain reg.

dataout  equ 20h  ; printer data register
p_status equ 21h  ; printer status register
                    ; 0 = printer not ready to accept new data yet
                    ;  1 = printer is ready for new data


; reads one line of characters from keyboard and send it
; to the printer character by character and also saves it
; in memory buffer calledc 'line'. 

 
section .text
        global _start

_start:  

keith:     mov edx, line     ; put address of line into edx
wait_key:  in al, k_status   ; input the status of the keyboard
           cmp al, 0
           jz wait_key      ; wait until a key is pressed
           in al , datain ; read it from keyboard and save inside bl
           mov bl, al      ; character saved inside bl
wait_print:in al, p_status  ; input status of printer
           cmp al, 0
           jz wait_print ; wait for printer to become ready
           mov al, bl    ; copy keybaord character into al to 
                         ; send it to the port
           out dataout, al ; send the character to the printer
           mov [edx], al     ; and also save it in memory buffer
           inc edx
           cmp al, 0dh       ; did we read the carriage return char yet?
           jnz wait_key      ; if not go read another character
           mov al, 0ah
           out dataout , al ; if yes send line feed to printer

           mov eax,1            ; The system call for exit (sys_exit)
           mov ebx,0            ; Exit with return code of 0 (no error)
           int 80h

