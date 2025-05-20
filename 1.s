# Файл: program.s
    .section .rodata
    .align 8
.LC0:                               # Строки формата
    .string "X\tY\n"
.LC1:
    .string "||%.2f||\t||%.2f||\n"
.LC2:                               # Константы
    .double -3.0
.LC3:
    .double -2.0
.LC4:
    .double 0.0
.LC5:
    .double 4.0
.LC6:
    .double 6.0
.LC7:
    .double 7.0
.LC8:
    .double 2.0
.LC9:
    .double 1.0
.LC10:
    .double 0.5
.LC11:
    .double -1.0
.LC12:
    .double 0.1
.LC13:
    .quad 0x7FF8000000000000        # NaN

    .text
    .globl calculateY
    .type calculateY, @function
calculateY:
    pushq   %rbp
    movq    %rsp, %rbp

    movsd   .LC2(%rip), %xmm1       # Загрузка -3.0
    ucomisd %xmm0, %xmm1
    ja      .L2                     

    movsd   .LC3(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jbe     .L2                     
    movsd   .LC11(%rip), %xmm1      
    mulsd   %xmm0, %xmm1            
    subsd   .LC8(%rip), %xmm1       
    movsd   %xmm1, %xmm0
    jmp     .Lend

.L2:
    movsd   .LC3(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    ja      .L3                     

    movsd   .LC4(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jbe     .L3                     
    addsd   .LC9(%rip), %xmm0       
    movsd   %xmm0, %xmm1
    mulsd   %xmm1, %xmm1            
    movsd   .LC9(%rip), %xmm0       
    subsd   %xmm1, %xmm0            
    sqrtsd  %xmm0, %xmm0            
    jmp     .Lend

.L3:
    movsd   .LC4(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    ja      .L4                     

    movsd   .LC5(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jb      .L4                     
    subsd   .LC8(%rip), %xmm0       
    movsd   %xmm0, %xmm1
    mulsd   %xmm1, %xmm1            
    movsd   .LC5(%rip), %xmm0       
    subsd   %xmm1, %xmm0            
    sqrtsd  %xmm0, %xmm0            
    mulsd   .LC11(%rip), %xmm0      
    jmp     .Lend

.L4:
    movsd   .LC5(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jae     .L5                     

    movsd   .LC6(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jb      .L5                     
    mulsd   .LC10(%rip), %xmm0      
    mulsd   .LC11(%rip), %xmm0      
    addsd   .LC8(%rip), %xmm0       
    jmp     .Lend

.L5:
    movsd   .LC6(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jae     .L6                     

    movsd   .LC7(%rip), %xmm1       
    ucomisd %xmm0, %xmm1
    jb      .L6                     
    movsd   .LC11(%rip), %xmm0      
    jmp     .Lend

.L6:                                
    movsd   .LC13(%rip), %xmm0

.Lend:
    popq    %rbp
    ret
    .size   calculateY, .-calculateY

    .globl  printResults
    .type   printResults, @function
printResults:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $64, %rsp
    movsd   %xmm0, -24(%rbp)        
    movsd   %xmm1, -32(%rbp)        
    movsd   %xmm2, -40(%rbp)        

    leaq    .LC0(%rip), %rdi        # Исправлено: RIP-relative адресация
    call    puts@PLT

    movsd   -24(%rbp), %xmm0
    movsd   %xmm0, -8(%rbp)         

.Lloop:
    movsd   -8(%rbp), %xmm0         
    movsd   -32(%rbp), %xmm1        
    ucomisd %xmm1, %xmm0
    ja      .Lexit                  

    movsd   -8(%rbp), %xmm0
    call    calculateY
    movsd   %xmm0, -48(%rbp)        

    ucomisd %xmm0, %xmm0            
    jp      .Lskip

    leaq    .LC1(%rip), %rdi        # Исправлено: RIP-relative адресация
    movsd   -8(%rbp), %xmm0         
    movsd   -48(%rbp), %xmm1        
    movl    $2, %eax                
    call    printf@PLT

.Lskip:
    movsd   -8(%rbp), %xmm0
    addsd   -40(%rbp), %xmm0        
    movsd   %xmm0, -8(%rbp)
    jmp     .Lloop

.Lexit:
    leave
    ret
    .size   printResults, .-printResults

    .globl  main
    .type   main, @function
main:
    pushq   %rbp
    movq    %rsp, %rbp

    movsd   .LC2(%rip), %xmm0       
    movsd   .LC7(%rip), %xmm1       
    movsd   .LC12(%rip), %xmm2      
    call    printResults

    movl    $0, %eax
    popq    %rbp
    ret
    .size   main, .-main
