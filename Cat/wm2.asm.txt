;whitespaces 9 10 11 12 13 32

sys_exit:       equ             60

                section         .text
                global          _start

buf_size:       equ             4
_start:
                xor             edx, edx
                xor             ebx, ebx
                sub             rsp, buf_size
                mov             rsi, rsp
                xor             rdx, rdx
                xor             rcx, rcx
read_again:
                sub             rdx, rcx
                xor             eax, eax
                xor             edi, edi
                mov             r8, rdx
                mov             rdx, buf_size
                syscall
                mov             rdx, r8
                
                test            rax, rax
                jz              quit
                js              read_error

                xor             rcx, rcx

check_char:
; rcx - current position in string
; rax - size of string
; rdx - last whitespace
                cmp             rcx, rax
                je              read_again
                cmp             byte [rsi + rcx], 32
                je             check
                cmp             byte [rsi + rcx], 9
                je             check
                cmp             byte [rsi + rcx], 10
                je             check
                cmp             byte [rsi + rcx], 11
                je             check
                cmp             byte [rsi + rcx], 12
                je             check
                cmp             byte [rsi + rcx], 13
                je             check
                inc             rcx
                jmp             check_char
check:
                cmp             rdx, rcx
                jne             increase
                
                inc             rcx
                mov             rdx, rcx
                
                jmp             check_char

quit:
                mov             rax, rbx
                call            print_int

                mov             rax, sys_exit
                xor             rdi, rdi
                syscall

; rax -- number to print
print_int:
                mov             rsi, rsp
                mov             ebx, 10

                dec             rsi
                mov             byte [rsi], 0x0a

next_char:
                xor             edx, edx
                div             ebx
                add             dl, '0'
                dec             rsi
                mov             [rsi], dl
                test            rax, rax
                jnz             next_char

                mov             eax, 1
                mov             edi, 1
                mov             rdx, rsp
                sub             rdx, rsi
                syscall

                ret

read_error:
                mov             eax, 1
                mov             edi, 2
                mov             rsi, read_error_msg
                mov             rdx, read_error_len
                syscall

                mov             rax, sys_exit
                mov             edi, 1
                syscall

                section         .rodata

increase:
                inc             rbx
                inc             rcx
                mov             rdx, rcx
                jmp             check_char
                
read_error_msg: db              "read failure", 0x0a
read_error_len: equ             $ - read_error_msg
