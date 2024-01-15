; Daniel Lam (40248073)
; SOEN 228 Winter 2023 - Assignment 1
; Tuesday, April 11, 2023

; Declare unassigned variables
section .bss
    max resb 1

; Declare initialized variables
section .data
  mick    db 3
  keith   db 4
  ron     db 2
  charlie db 1
  

section .text
	global _start

_start:

  ; CODE LOGIC:
  ; 1. Assign first two .data variables to al and bl which will be used as 
  ; 2. compareMax stores larger of both values into dl register
  ; 3. dl keeps getting assigned to al while bl takes in values of every other variable
  ; aka, dl will always store the largest value of all variables

  ; Assign al and bl to first two variables
  mov al, [mick]
  mov bl, [keith]
  call compareMax
  
  ; Compare dl with ron
  mov al, dl
  mov bl, [ron]
  call compareMax
  
  ; Compare dl with charlie
  mov al, dl
  mov bl, [charlie]
  call compareMax
  
  
  ; Assign dl to value of max
	mov [max], eax
	
	; Print max
	call PrintMax
	
	mov eax, 1
	mov ebx, 0
	int 80h

; PrintMax prints the max integer to the console
PrintMax:
  mov dl, [max]
  add dl, 48
  mov [max], dl
  
  mov eax, 4
  mov ebx, 1
  mov ecx, max
  mov edx, 1

	int 80h;
	
	ret

; compareMax assigns dl to largest value between al and bl
compareMax:
  ; if (al > bl)
  cmp al, bl
  ja al_bigger
  ; else dl = bl
  mov dl, bl
  ret
  
al_bigger:
  ; dl = al
  mov dl, al
  ret
