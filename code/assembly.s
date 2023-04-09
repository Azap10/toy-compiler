
.globl foo
foo:
pushq %rbp
movq %rsp, %rbp
pushq %rdi
pushq %rsi
movq -8(%rbp), %r8
movq -16(%rbp), %rax
addq %r8, %rax
pushq %rax
movq -24(%rbp), %rax
pushq %rax
movq -32(%rbp), %rax
addq $0x20, %rsp
popq  %rbp
retq
