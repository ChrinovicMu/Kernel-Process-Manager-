	.file	"main.c"
	.intel_syntax noprefix
	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"Error allocating kernel info memory"
	.align 8
.LC1:
	.string	"memory allocation of process failed\n"
.LC2:
	.string	"Couldn't push to the stack\n"
.LC3:
	.string	"BEFORE KILL:"
.LC4:
	.string	"kernel stack memory : %zu\n"
.LC5:
	.string	"kernel memory used : %zu\n\n"
.LC6:
	.string	"AFTER KILL:"
.LC7:
	.string	"AFTER CLEANUP:"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 112
	mov	DWORD PTR -100[rbp], edi
	mov	QWORD PTR -112[rbp], rsi
	mov	edi, 16
	call	malloc@PLT
	mov	QWORD PTR -8[rbp], rax
	cmp	QWORD PTR -8[rbp], 0
	jne	.L2
	mov	rax, QWORD PTR stderr[rip]
	mov	rcx, rax
	mov	edx, 35
	mov	esi, 1
	lea	rax, .LC0[rip]
	mov	rdi, rax
	call	fwrite@PLT
	mov	edi, 1
	call	exit@PLT
.L2:
	mov	rax, QWORD PTR -8[rbp]
	mov	QWORD PTR [rax], 1024
	mov	rax, QWORD PTR -8[rbp]
	mov	QWORD PTR 8[rax], 0
	mov	QWORD PTR -16[rbp], 0
	mov	QWORD PTR -24[rbp], 0
	mov	QWORD PTR -32[rbp], 0
	mov	QWORD PTR -40[rbp], 0
	mov	edi, 1111
	call	create_process@PLT
	mov	QWORD PTR -32[rbp], rax
	mov	edi, 2222
	call	create_process@PLT
	mov	QWORD PTR -40[rbp], rax
	mov	edi, 3333
	call	create_process@PLT
	mov	QWORD PTR -16[rbp], rax
	mov	edi, 4444
	call	create_process@PLT
	mov	QWORD PTR -24[rbp], rax
	cmp	QWORD PTR -16[rbp], 0
	je	.L3
	cmp	QWORD PTR -24[rbp], 0
	je	.L3
	cmp	QWORD PTR -32[rbp], 0
	je	.L3
	cmp	QWORD PTR -40[rbp], 0
	jne	.L4
.L3:
	mov	rax, QWORD PTR stderr[rip]
	mov	rcx, rax
	mov	edx, 36
	mov	esi, 1
	lea	rax, .LC1[rip]
	mov	rdi, rax
	call	fwrite@PLT
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -24[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -32[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -40[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	edi, 1
	call	exit@PLT
.L4:
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -16[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	push_p@PLT
	mov	DWORD PTR -44[rbp], eax
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -24[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	push_p@PLT
	mov	DWORD PTR -48[rbp], eax
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -32[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	push_p@PLT
	mov	DWORD PTR -52[rbp], eax
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -40[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	push_p@PLT
	mov	DWORD PTR -56[rbp], eax
	cmp	DWORD PTR -44[rbp], 0
	jne	.L5
	cmp	DWORD PTR -48[rbp], 0
	jne	.L5
	cmp	DWORD PTR -52[rbp], 0
	jne	.L5
	cmp	DWORD PTR -56[rbp], 0
	je	.L6
.L5:
	mov	rax, QWORD PTR stderr[rip]
	mov	rcx, rax
	mov	edx, 27
	mov	esi, 1
	lea	rax, .LC2[rip]
	mov	rdi, rax
	call	fwrite@PLT
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -24[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -32[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -40[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	edi, 1
	call	exit@PLT
.L6:
	mov	rax, QWORD PTR -16[rbp]
	mov	QWORD PTR -96[rbp], rax
	mov	rax, QWORD PTR -24[rbp]
	mov	QWORD PTR -88[rbp], rax
	mov	rax, QWORD PTR -32[rbp]
	mov	QWORD PTR -80[rbp], rax
	mov	rax, QWORD PTR -40[rbp]
	mov	QWORD PTR -72[rbp], rax
	mov	QWORD PTR -64[rbp], 4
	mov	rdx, QWORD PTR -64[rbp]
	lea	rax, -96[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	run_process_threads@PLT
	lea	rax, .LC3[rip]
	mov	rdi, rax
	call	puts@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR [rax]
	mov	rsi, rax
	lea	rax, .LC4[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR 8[rax]
	mov	rsi, rax
	lea	rax, .LC5[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -16[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	kill_p@PLT
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -24[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	kill_p@PLT
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -32[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	kill_p@PLT
	mov	rdx, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR -40[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	kill_p@PLT
	lea	rax, .LC6[rip]
	mov	rdi, rax
	call	puts@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR [rax]
	mov	rsi, rax
	lea	rax, .LC4[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR 8[rax]
	mov	rsi, rax
	lea	rax, .LC5[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	mov	eax, 0
	call	clean_memory_blocks@PLT
	lea	rax, .LC7[rip]
	mov	rdi, rax
	call	puts@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR [rax]
	mov	rsi, rax
	lea	rax, .LC4[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rax, QWORD PTR 8[rax]
	mov	rsi, rax
	lea	rax, .LC5[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	call	free@PLT
	mov	eax, 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (Debian 12.2.0-14) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
