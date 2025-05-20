# poly.s
.intel_syntax noprefix
.globl main

# Структура Polynomial
.set COEFFS_OFFSET, 0
.set SIZE_OFFSET, 8
.set POLY_STRUCT_SIZE, 16

# Системные вызовы
.set SYS_WRITE, 1
.set SYS_EXIT, 60

.section .rodata
inf_str: .string "infinity\n"
result_fmt: .string "%lld/%lld\n"
scanf_fmt: .string "%d %d"

.section .text

# long long gcd_ll(long long a, long long b)
gcd_ll:
    push rbp
    mov rbp, rsp
    
    # Абсолютные значения
    mov rax, rdi
    mov rdx, rsi
    test rax, rax
    jns .Labs_a
    neg rax
.Labs_a:
    test rdx, rdx
    jns .Labs_b
    neg rdx
.Labs_b:
    
    # Алгоритм Евклида
.Lgcd_loop:
    test rdx, rdx
    je .Lgcd_done
    xor rcx, rcx
    div rdx
    mov rax, rdx
    mov rdx, rcx
    jmp .Lgcd_loop
    
.Lgcd_done:
    pop rbp
    ret

# Polynomial* poly_alloc(size_t size)
poly_alloc:
    push rbp
    mov rbp, rsp
    push rbx
    
    # Выделяем память под структуру
    mov rdi, POLY_STRUCT_SIZE
    call malloc
    mov rbx, rax
    
    # Выделяем память под коэффициенты
    mov rdi, [rbp + 16]
    shl rdi, 3
    call malloc
    mov [rbx + COEFFS_OFFSET], rax
    
    # Инициализируем размер
    mov rax, [rbp + 16]
    mov [rbx + SIZE_OFFSET], rax
    
    mov rax, rbx
    pop rbx
    pop rbp
    ret

# void poly_free(Polynomial *p)
poly_free:
    push rbp
    mov rbp, rsp
    mov rdi, [rdi + COEFFS_OFFSET]
    call free
    pop rbp
    ret

# Polynomial* derivative(const Polynomial *p)
derivative:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    
    mov r12, rdi
    mov rax, [r12 + SIZE_OFFSET]
    dec rax
    push rax
    call poly_alloc
    add rsp, 8
    
    mov r13, rax
    mov rcx, [r12 + SIZE_OFFSET]
    dec rcx
    mov rsi, [r12 + COEFFS_OFFSET]
    mov rdi, [r13 + COEFFS_OFFSET]
    
    xor rdx, rdx
.Lderiv_loop:
    cmp rdx, rcx
    jge .Lderiv_done
    mov rax, [rsi + 8 + rdx*8]
    imul rax, rdx
    inc rax
    mov [rdi + rdx*8], rax
    inc rdx
    jmp .Lderiv_loop
    
.Lderiv_done:
    pop r13
    pop r12
    pop rbp
    ret

# Остальные функции (polyAdd, polySub и т.д.) реализуются аналогично
# ...

main:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    
    # Чтение a и b
    lea rdi, scanf_fmt[rip]
    lea rsi, [rbp - 8]
    lea rdx, [rbp - 16]
    xor eax, eax
    call scanf
    
    # Проверка b == 1
    cmp DWORD PTR [rbp - 16], 1
    jne .Lnot_inf
    lea rdi, inf_str[rip]
    call printf
    jmp .Lexit
    
.Lnot_inf:
    # Инициализация P0
    mov edi, 2
    call poly_alloc
    mov [rbp - 24], rax
    mov rdi, [rax + COEFFS_OFFSET]
    mov QWORD PTR [rdi], 0
    mov QWORD PTR [rdi + 8], 1
    
    # Основной цикл построения Pa
    mov DWORD PTR [rbp - 28], 1
.Lloop_k:
    # ... реализация рекурсивного построения ...
    inc DWORD PTR [rbp - 28]
    mov eax, [rbp - 28]
    cmp eax, [rbp - 8]
    jle .Lloop_k
    
    # Вычисление Pa(1/b)
    # ...
    
    # Вывод результата
    mov rsi, [rbp - 40] # numFinal
    mov rdx, [rbp - 48] # denFinal
    lea rdi, result_fmt[rip]
    xor eax, eax
    call printf
    
.Lexit:
    add rsp, 48
    pop rbp
    xor eax, eax
    ret

# Импорт функций из libc
.extern printf
.extern scanf
.extern malloc
.extern free
