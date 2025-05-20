.intel_syntax noprefix
.globl main

.section .rodata
scanf_fmt: .string "%d %d"
printf_fmt: .string "%lld\n"
output_fmt: .string "%d\n"

.section .bss
arr:         .space 1000008
prefixSum:   .space 1000008
dp:          .space 1000008

.section .text
main:
    push rbp
    mov rbp, rsp
    
    # Чтение n и m
    lea rdi, scanf_fmt[rip]
    lea rsi, [rbp-8]       # n
    lea rdx, [rbp-16]      # m
    xor eax, eax
    call scanf
    
    # Чтение массива
    mov ecx, [rbp-8]
    test ecx, ecx
    jle .Lread_done
    xor ebx, ebx
.Lread_loop:
    lea rdi, scanf_fmt[rip]
    lea rsi, arr[rip+rbx*8]
    xor eax, eax
    call scanf
    inc ebx
    cmp ebx, [rbp-8]
    jl .Lread_loop
.Lread_done:

    # Инициализация prefixSum
    mov ecx, [rbp-8]
    lea rsi, prefixSum[rip+8]
    lea rdi, arr[rip]
    xor eax, eax
    mov [rsi-8], rax
.Lprefix_loop:
    mov rax, [rsi-8]
    add rax, [rdi]
    mov [rsi], rax
    add rdi, 8
    add rsi, 8
    dec ecx
    jnz .Lprefix_loop

    # Инициализация dp
    mov ecx, [rbp-8]
    inc ecx
    lea rdi, dp[rip]
    xor eax, eax
    rep stosq

    # Заполнение dp
    mov ecx, [rbp-8]
    dec ecx
    jl .Ldp_done
.Ldp_outer:
    # i = ecx
    mov edx, 1             # k = 1
    mov r8d, [rbp-16]      # m
    mov r9, -0x7FFFFFFFFFFFFFFF # best
    lea rsi, dp[rip]
    lea rdi, prefixSum[rip]
.Ldp_inner:
    # Проверка i + k <= n
    mov eax, ecx
    add eax, edx
    cmp eax, [rbp-8]
    jg .Linner_done

    # sumTaken = prefixSum[i + k] - prefixSum[i]
    mov rax, [rdi + rax*8]
    sub rax, [rdi + rcx*8]
    
    # candidate = sumTaken - dp[i + k]
    mov r10, [rsi + rax*8]
    sub rax, r10
    
    # Обновление best
    cmp rax, r9
    cmovg r9, rax
    
    inc edx
    cmp edx, r8d
    jle .Ldp_inner
.Linner_done:
    mov [rsi + rcx*8], r9  # dp[i] = best
    dec ecx
    jns .Ldp_outer
.Ldp_done:

    # Проверка dp[0] > 0
    mov rax, dp[rip]
    test rax, rax
    jle .Lzero
    mov eax, 1
    jmp .Lprint
.Lzero:
    xor eax, eax
.Lprint:
    lea rdi, output_fmt[rip]
    mov esi, eax
    xor eax, eax
    call printf

    xor eax, eax
    leave
    ret
