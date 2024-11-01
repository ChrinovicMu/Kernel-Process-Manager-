#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define MEMORY_LIMIT 1024

// Core structures
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

struct Mem_Block {
    struct Process* process;
    struct Mem_Block* next_b;
    struct Mem_Block* previous_b;
};

struct Kernel_Info {
    size_t kernel_stack_mem;
    size_t kernel_stack_used;
};

// Function declarations
struct Process* creatProcess(unsigned int id);
int push_P(struct Process* p, struct Kernel_Info* kernel_stack_info);
void run_P(struct Process* p);
void kill_P(struct Process* p, struct Kernel_Info* kernel_stack_info);
void clean_memoryBlocks(struct Kernel_Info* kernel_stack_info);
void run_process_threads(struct Process* p_array[], size_t len);

#endif
