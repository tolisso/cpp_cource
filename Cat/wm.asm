; 9 10 11 12 13 32

sys_exit:       equ             60

                section         .text
                global          _start

buf_size:       equ             8192
_start:
                xor             ebx, ebx
                sub             rsp, buf_size
                mov             rsi, rsp
                xor             edx, edx
read_again:
; rdx - pos of prev whitespace + 1
;                 cmp             rdx, rcx
;                 jne             add_last_word
                
                xor             edx, edx
                xor             eax, eax
                xor             edi, edi
                mov             rdx, buf_size
                syscall

                test            rax, rax
                jz              quit
                js              read_error

                xor             ecx, ecx


check_char:
; rcx - current position in string
; rax - size of string
;                 cmp             rdx, rcx
;                 jne             check_word
                cmp             rcx, rax
                je              read_again
                
                cmp             byte [rsi + rcx], 9
                jne             check_word
                cmp             byte [rsi + rcx], 10
                jne             check_word
                cmp             byte [rsi + rcx], 11
                jne             check_word
                cmp             byte [rsi + rcx], 12
                jne             check_word
                cmp             byte [rsi + rcx], 13
                jne             check_word
                cmp             byte [rsi + rcx], 32
                jne             check_word
                inc             rcx
                
skip:
                inc             rcx
                jmp             check_char

quit:
                mov             rax, rbx
                call            print_int

                mov             rax, sys_exit
                xor             rdi, rdi
                syscall

                
add_last_word:
                inc             rbx
                xor             rcx, rcx
                xor             rdx, rdx
                jmp             read_again
                
check_word:
                cmp             rdx, rcx
                jne             increase
                
                mov             rdx, rcx
                xor             rdx, rdx
                mov             rdx, rcx
                inc             rdx
                inc             rcx
                jmp             check_char

increase:       
                inc             rbx;

                

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

read_error_msg: db              "read failure", 0x0a
read_error_len: equ             $ - read_error_msg
