; Ted Obuchowicz
                            
                              
 section .data
                                     
; put your data in this section using
; db , dw, dd directions
                                       
my_list db 1,2,3,4,5

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
                                      
        mov eax, my_list     ;  In INTEL terminology, this is called immediate addressing
                             ; eax will be loaded with memory address
                             ; of label "mylist"
                             ; some textbooks and other assemblers for different
                             ; processors use the notation mov dest_reg, # <some_immediate_data>
                             ; to specify IMMEDIATE mode

                                      
ron:    mov bl, [my_list]    ; Intel calls this DIRECT addressing
                             ; others refer to it as ABSOLUTE and typically
                             ; use a different notational syntax .. this is
                             ; often a source of confusion . ie.
                             ; sometime textbooks will employ the syntax
                             ; mov dest_register , my_list  to designate
                             ; absolute addressing
                             ; bl will be loaded with the 1 byte from
                             ; memory location my_list (i.e. with value 1)
                                      
                                    
        mov cl, [eax]         ; register indirect addressing
                              ; cl will be loaded with 1 byte of date from
                              ; memory location whose address is contained in
                              ; register eax  (i.e. cl will also be loaded with 1)
                                     
       mov edx, 12345678h    ;   INTEL immediate addressing
                             ; edx will be loaded with the hex data 12345678
                             ; which is found in the successive byte immediately
                             ; following the byte specifying the opcode for mov ebx
                             ; note for INTEL this immediate data is stored BACKWARDS
                             ; 
                             ;  OPCODE
                             ;  78
                             ;  56
                             ;  34
                             ;  12
                             ; thus this is a 5 byte long instruction stored in memory
                             ; note in INTEL assembly the # symbol is not needed to specify
                             ; immediate mode addressing... it's presence in an instruction
                             ; would most likely cause an error ...

;       mov al, #5           ; yes indeed this line causes an error:
                             ; more_intel_addressing_modes.asm:66: error: expression syntax error 
                             ; so you see when learning assembly language, half the battle is
                             ; in getting silly syntactical notation straightened out... 
                                     
                                     
; BASED and INDEXED addressing
; these are fancy names for register indirect  
; when we use "special" registers:
; INTEL calls this mode BASED addressing when we use:
; BX or EBX
; BP or EPB
; 
; and they are called INDEXED addressing when we use:
; SI or ESI 
; DI or EDI
                                     
         mov ebp, my_list
         mov al, [ebp]       ; al loaded with 1
         mov esi, my_list
         mov cl , [esi]
                                      
; a DISPLACEMENT can be added to the contents of the
; specified BASE or INDEX register
                                    
         mov  al, [ebp + 4]  ; al will be loaded with the 5th element of my_list
                                     
; most assemblers allow for the following equivalent way of
; writing this base + displacement
                                       
;        mov bl, 4[ebp]      ; bl gets loaded with the 5th byte of the list 
                             ; found in my_list[4] using Java array notation
                             ; BUT NOT NASM AS IT GIVES AN ERROR !!!
                                      
;  can even specify a SYMBOLIC LABEL as the DISPLACEMENT VALUE
; and use either a BASE or an INDEX register to get each
; element in the list (by adding 1 to the BASE or INDEX register 
; being used)  add 2 if using word size operands, 4 for double words
                                      
        mov ebp , 0
;       mov dl, my_list[ebp]     ; get the first element of the list
                                 ; use the NASM friendly syntax 
        mov dl, [my_list + ebp]
        inc ebp                  ; make bp point to the next element
        mov dl, [my_list + ebp]  ; get the 2nd element
        inc ebp
        mov dl, [my_list+ebp]    ; get the 3rd element
        inc ebp
        mov dl, [my_list + ebp]  ; get the 4th element
                                     
                                      
; BASED-INDEXED ADDRESSING
; another fancy word for good old register indirect when we
; use the contents of a BASE register added together with the
; contents of an INDEX register to obtain the memory address of the
; operand
                                    
       mov esi, my_list      ; load esi with starting address of my_list
       mov ebp, 0
       mov al, [esi + ebp]   ; get the 1st element
       inc ebp
;     mov al, [ebp][esi]     ; same as saying [esi + ebp]
                             ; will get the second element
                             ; NASM DOESN'T LIKE THIS SYNTAX
                             ; so use 
      mov al, [ebp + esi]    ; this notation
      inc ebp
      mov al, [ebp + esi]    ; get the 3rd element
                             ; etcetera
                             ; ad nauseum
                                     
; TOO MANY ADDRESSING MODES SPOILS THE ASSEMBLY LANGUAGE PROGRAMMER
; Intel even allows for a DISPLACEMENT to be specified when using
; BASED-INDEXED ADDRESSING to come up with something their engineers
; call "BASED-INDEXED WITH DISPLACEMENT" which is yet another fancy
; variation of good old register indirect addressing:
                                    
       mov esi, 0
       mov ebp, 0
;      mov al, my_list[ebp][esi]       ; al gets loaded with 1st 
                                       ; element of list
                                       ; use the NASM friendly syntax
       mov al, [my_list + ebp + esi]
       inc esi
       mov al, [my_list + ebp + esi]   ; al gets loaded with 2nd element
       inc esi
       mov al, [my_list + ebp + esi]   ; get 3rd element of list
                                     
                                  
       mov eax,1            ; The system call for exit (sys_exit)
       mov ebx,0            ; Exit with return code of 0 (no error)
       int 80h
                                    
