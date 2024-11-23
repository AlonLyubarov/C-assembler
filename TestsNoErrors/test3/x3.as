.entry LIST
.extern fn1
MAIN: add r3, LIST
STR: .string "abcd"
jsr fn1
.extern good
LOOP: prn #48
 lea STR, r6
 inc r6
 .entry ALON
 END: stop
 bne good
 HI: inc r5
 K: .data 31
 mov *r6, L3
 sub r1, r4
 .entry HI
 ALON: .string "AAAA"
 cmp r3, #-6
 bne END
 lea ALON, r5
 .extern L3 
 .data 100
 .data 8, 7
 add r7, *r6
 clr K
 sub L3, L3
.entry MAIN
 jmp LOOP
LIST: .data 6, -9
 .data -100
 mov *r5, L3
 sub r2, r3
 cmp r2, #-8
 bne LIST

