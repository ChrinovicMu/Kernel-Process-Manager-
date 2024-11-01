#ifndef PROLIB_H
#define PROLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define MEMORY_LIMIT 1024
#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)

struct Mem_Block {
    struct Process* process;
    struct Mem_Block *next_b;
    struct Mem_Block *previous_b;
};

struct Kernel_Info {
    size_t kernel_stack_mem;
    size_t kernel_stack_used;
};

struct Context {
    uint32_t eip;
    uint32_t esp;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
};

enum P_State {
    READY,
    RUNNING,
    SLEEPING,
    UNUSED,
    FINISHED,
};

struct Process {
    size_t size;
    enum P_State state;
    unsigned int pid;
    int slp : 1;
    int kill: 1;
    struct Context context; 
};

void add_to_stack(struct Mem_Block *b);
int push_p(struct Process * p, struct Kernel_Info * kernel_stack_info);
void run_p(struct Process *p);
void *thread_func(void *arg);
void run_process_threads(struct Process *p_array[], size_t len);
struct Process * create_process(unsigned int id);
void kill_p(struct Process *p, struct Kernel_Info *kernel_stack_info);
void clean_memory_blocks();
void rr_schedule();

#endif
