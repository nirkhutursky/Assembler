MAIN: add r3, LIST
LOOP: prn #48
macr m_macr
cmp r3, #-6
bne END
endmacr
lea STR, r6
inc r6
STR: .string "abcd"
mov *r6,K
sub r1,r2
m_macr
dec K
jmp LOOP
END: stop
LIST: .data    6   ,        -9
.data -100
K: .data 31
