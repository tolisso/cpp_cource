                section         .text

                global          _start

_start:
                mov             rsi, buf
                
read_again:
                mov             rax, 1
                xor             eax, eax
                mov             rdi, 1
                mov             rdx, buf_size
                syscall

                test            rax, rax
                jz              quit
                
                mov             rdx, rax
                
                
                
                mov             rax, 1
                mov             rdi, 1
                syscall
                
                jmp             read_again
                
quit:
                mov             rax, 60
                xor             rdi, rdi
                syscall
                
                section         .rodata

msg:            db              "Hello, world!",0x0a
msg_size:       equ             $ - msg

                section         .bss

buf_size:       equ             8192
buf:            resb            buf_size
                
