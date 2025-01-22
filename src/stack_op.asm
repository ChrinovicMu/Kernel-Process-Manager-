section .text
global asm_add_to_stack

asm_add_to_stack:
    ; Prefetch the next likely memory locations
    prefetchw [rsi]           ; Prefetch top pointer for writing
    prefetchw [rdi + 8]       ; Prefetch next_b field
    
    ; Load top and do NULL check in one operation
    mov rax, [rsi]
    test rax, rax
    jz .empty_stack
    
    ; Use SIMD to set both next_b and previous_b in one operation
    pxor xmm0, xmm0          ; Zero XMM register
    movdqu [rdi + 8], xmm0   ; Set both next_b and previous_b to NULL atomically
    
    ; Update pointers with minimal cache misses
    mov [rdi + 8], rax       ; new->next_b = top
    mov [rax + 16], rdi      ; top->previous_b = new
    mov [rsi], rdi           ; top = new
    ret

.empty_stack:
    ; Use SIMD for empty case too
    pxor xmm0, xmm0
    movdqu [rdi + 8], xmm0   ; Set both pointers to NULL in one operation
    mov [rsi], rdi           ; top = new
    ret
