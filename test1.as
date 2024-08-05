MAIN: add r3, LIST
Data : porn #48
macr m_macr
 cmp r3, #-6
 bne END
endmacr
 lea STR, r6
 inc r6
 mov *r6,K
    subi r1, r4
 m_macr
 dec K
 jmp LOOP
END: stop
    STR:    .string    "abcd"