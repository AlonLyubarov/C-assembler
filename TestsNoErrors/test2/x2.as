.entry LIST
.extern fn1
MAIN: add r3, LIST
jsr fn1
macr alon
LOOP: prn #48
 lea STR, r6
 inc r6
 mov *r6, L3
 sub r1, r4
 endmacr
 alon
 cmp r3, #-6
 bne END
 add r7, *r6
 .data -100
 clr K
 LIST: .data 6, -9
 STR: .string "abcd"
 sub L3, L3
.entry MAIN
 jmp LOOP
END: stop
K: .data 31
.extern L3 
