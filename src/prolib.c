#include "prolib.h"
#include <stdbool.h>
#include <unistd.h>

struct Mem_Block *top = NULL;

void add_to_stack(struct Mem_Block *b){
    if(top == NULL){
        top = b;
        b->next_b = NULL;
        b->previous_b = NULL;
    }else {
        b->next_b = top;
        b->previous_b = NULL;
        top->previous_b = b;
        top = b;
    }
}
int push_p(struct Process * p, struct Kernel_Info * kernel_stack_info){

    if (p == NULL || kernel_stack_info == NULL){
        fprintf(stderr, "invalid pointers\n");
        return -1;
    }
    struct Mem_Block *b = (struct Mem_Block *) malloc(sizeof(struct Mem_Block));
    if (!b){
        fprintf(stderr,"allocation failed\n");
        return -1; 
    }

    if (p->state !=READY){
        free(b);
        fprintf(stderr, "process not ready for execution\n");
        return -1;
    }
    if((kernel_stack_info->kernel_stack_used + MEM_BLOCK_SIZE + p->size) > (kernel_stack_info->kernel_stack_mem)){
        free(b);
        fprintf(stderr, "Memory Full\n");
        return -1;
    }
    kernel_stack_info->kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
    b->process = p;
    add_to_stack(b);
    return 0;
}
struct Process * create_process(unsigned int id, uint32_t burst_time){

    if(id == 0){
        fprintf(stderr, "invalid process id\n");
        return NULL;
    }

    //arbitrary test values
    struct Context context = {0};
    context.eip = 12;
    context.esp = 13;
    context.eax = 54;
    context.ebx = 5;

    struct Process *process = (struct Process *)malloc(sizeof(struct Process));
    if(!process){
        fprintf(stderr, "Process memory allocation failed\n");
        return NULL;
    }
    process->size = sizeof(struct Process);
    process->state = READY;
    process->pid = id;
    process->context = context;
    process->burst_time = burst_time;
    return (process);
}
//kill single process and free its memory block 
void kill_p(struct Process *p, struct Kernel_Info *kernel_stack_info){ 

    if(p == NULL || kernel_stack_info == NULL){
        fprintf(stderr, "invalid process ptr\n");
        return;
    }
    struct Mem_Block *current = top;
    struct Mem_Block *prev = NULL;

    while(current != NULL){

        if(current->process != NULL && current->process->pid == p->pid){
            if (prev == NULL){
                top = current->next_b;
            }else{
                prev->next_b = current->next_b;
            }

            kernel_stack_info->kernel_stack_used -= (MEM_BLOCK_SIZE + p->size);
            free(current->process);
            free(current);

            return;
        }
        prev = current;
        current = current->next_b;
    }
    fprintf(stderr, "Process not in mem block\n");
}
void clean_memory_blocks(struct Kernel_Info *kernel_stack_info){

    if (kernel_stack_info == NULL){
        fprintf(stderr, "invalid kernel_stack_info\n");
        return;
    } 
    if(top == NULL){
        printf("no blocks memory blocks to clear\n");
        return;
    }
    struct Mem_Block *current = NULL;
    current = top;

    while (current != NULL){
        struct Mem_Block *next = current->next_b;

        if(current->process != NULL){
            kernel_stack_info->kernel_stack_used -= current->process->size;
            free(current->process);
        }
        kernel_stack_info->kernel_stack_used -= MEM_BLOCK_SIZE;
        free(current);
        current = next;
    }
    top = NULL;
    printf("All memory blocks cleared\n");
}
struct MLFQ *init_mlfq(void) {
    struct MLFQ *mlfq = (struct MLFQ*)malloc(sizeof(struct MLFQ));
    if(!mlfq){
        fprintf(stderr, "Failed to allocate MLFQ\n");
        return NULL;
    }
    for(size_t x = 0; x < MAX_QUEUES; ++x) {
        mlfq->queues[x].count = 0;
        mlfq->queues[x].quantum = 1 << x;
    }
    mlfq->current_queue = 0;
    return mlfq;
}
void add_process(struct MLFQ *mlfq, struct Process* process) {
    if(!mlfq || !process){
        return;
    }

    process->remaining_time = process->burst_time;
    process->queue_level = 0;
    process->quantum_used = 0;
    process->state = READY;

    struct Queue *top_queue = &mlfq->queues[0];
    if (top_queue->count >= MAX_PROCESSES) {
        fprintf(stderr, "Queue is full\n");
        return;
    }
    top_queue->process_arr[top_queue->count++] = process;
}
static void demote_process(struct MLFQ *mlfq, struct Process *process, int current_queue_level) {

    if (current_queue_level + 1 < MAX_QUEUES) {
        process->queue_level = current_queue_level + 1;
        process->quantum_used = 0;
        struct Queue *next_queue = &mlfq->queues[current_queue_level + 1];
        next_queue->process_arr[next_queue->count++] = process;
        printf("Process %d demoted to queue %d\n", process->pid, current_queue_level + 1);
    }
}

void execute_time_slice(struct MLFQ *mlfq) {
    if (!mlfq) {
        return;
    }

    for (size_t x = 0; x < MAX_QUEUES; x++) {
        struct Queue *current_queue = &mlfq->queues[x];

        if (current_queue->count > 0) {

            // Validate the process pointer
            if (current_queue->process_arr[0] == NULL) {
                fprintf(stderr, "Invalid process pointer\n");
                return;
            }

            struct Process *process = current_queue->process_arr[0];

            // Check if process is ready for execution
            if (process->state != READY) {
                fprintf(stderr, "Not ready for execution\n");
                continue;
            }

            printf("PID %d is RUNNING\n", process->pid);

            sleep(2);
            // Execute the process
            process->state = RUNNING;
            process->remaining_time--;
            process->quantum_used++;

            // Handle process completion
            if (process->remaining_time == 0) {
                printf("Process %d completed\n", process->pid);
                process->state = FINISHED;

                // Remove the process from the queue
                for (int i = 0; i < current_queue->count - 1; ++i) {
                    current_queue->process_arr[i] = current_queue->process_arr[i + 1];
                }
                current_queue->count--;
                free(process);
            }
            // Handle process quantum expiration
            else if (process->quantum_used >= current_queue->quantum) {
                printf("Process %d used up its quantum, moving to next queue\n", process->pid);
                process->state = READY; // Reset state before demotion
                process->quantum_used = 0; // Reset quantum usage

                // Remove process from the queue
                for (int i = 0; i < current_queue->count - 1; ++i) {
                    current_queue->process_arr[i] = current_queue->process_arr[i + 1];
                }
                current_queue->process_arr[current_queue->count - 1] = NULL;
                current_queue->count--;

                // Demote process to the next queue
                demote_process(mlfq, process, x);
            }
            // Process remains in the queue for further execution
            else {
                process->state = READY;
            }

            // Only process one process per time slice
            break;
        }
    }
}
