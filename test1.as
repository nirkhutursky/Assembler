MAIN: add r3, LIST
LOOP : prn #48
lea STR, r6
inc r6
mov *r6,K
sub r1, r4
cmp r3, #-6
bne END
dec K
jmp LOOP
END: stop
STR:    .string    "abcd"
