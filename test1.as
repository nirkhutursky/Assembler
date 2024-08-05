MAIN: add r3, LIST
LOOP: prn #48
macr m_macr
 cmp r3, #-6
 bne END
endmacr
 lea STR, r6
 inc r6
 mov *r6,K
 sub r1, r4
 m_macr
 dec K
 jmp LOOP
r55: stop
   ;STR: .string "abcd"
DD: .string
LIST: .data    6   ,        -9, +17
 .data -100
 stop