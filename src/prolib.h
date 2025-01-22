#ifndef PROLIB_H
#define PROLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define MEMORY_LIMIT 1024
#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)
#define MAX_PROCESSES 4
#define MAX_QUEUES 100

struct Queue {
    struct Process *process_arr[MAX_PROCESSES];
    int count;
    int quantum;
};
struct MLFQ {
    struct Queue queues[MAX_QUEUES];
    int current_queue;
};
struct Mem_Block {
    struct Process* process;
    struct Mem_Block *next_b;
    struct Mem_Block *previous_b;
};
struct Kernel_Info {
    size_t kernel_stack_mem;
    size_t kernel_stack_used;
};

//process registers content 
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
    struct Context context;
    uint32_t burst_time;
    uint32_t remaining_time;
    uint32_t queue_level;
    uint32_t quantum_used;
};

extern void asm_add_to_stack(struct Mem_Block *new_block, struct Mem_Block **top);
struct MLFQ *init_mlfq(void);
void add_process(struct MLFQ *mlfq, struct Process *process);
void execute_time_slice(struct MLFQ *mlfq);
void add_to_stack(struct Mem_Block *b);
int push_p(struct Process * p, struct Kernel_Info * kernel_stack_info);
void run_p(struct Process *p);
struct Process * create_process(unsigned int id, uint32_t bur);
void kill_p(struct Process *p, struct Kernel_Info *kernel_stack_info);
void clean_memory_blocks();

#endif
