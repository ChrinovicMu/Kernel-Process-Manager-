#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "prolib.h"

// Declare top as extern to access the global variable defined in prolib.c
extern struct Mem_Block *top;

int main(int argc, char *argv[]) {
    // Initialize kernel stack info
    struct Kernel_Info *kernel_stack_info = (struct Kernel_Info*) malloc(sizeof(struct Kernel_Info));
    if (kernel_stack_info == NULL) {
        fprintf(stderr, "Error allocating kernel info memory");
        exit(EXIT_FAILURE);
    }
    kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT;
    kernel_stack_info->kernel_stack_used = 0;

    // Initialize MLFQ scheduler
    struct MLFQ *scheduler = init_mlfq();
    if (!scheduler) {
        fprintf(stderr, "Failed to initialize scheduler\n");
        free(kernel_stack_info);
        exit(EXIT_FAILURE);
    }

    // Create processes
    struct Process *p1 = create_process(1111, 10);
    struct Process *p2 = create_process(2222, 8);
    struct Process *p3 = create_process(3333, 4);
    struct Process *p4 = create_process(4444, 7);

    if (!p1 || !p2 || !p3 || !p4) {
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(kernel_stack_info);
        free(scheduler);
        exit(EXIT_FAILURE);
    }

    // Push processes to stack
    int push1 = push_p(p1, kernel_stack_info);
    int push2 = push_p(p2, kernel_stack_info);
    int push3 = push_p(p3, kernel_stack_info);
    int push4 = push_p(p4, kernel_stack_info);

    if (push1 != 0 || push2 != 0 || push3 != 0 || push4 != 0) {
        fprintf(stderr, "Couldn't push to the stack\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(kernel_stack_info);
        free(scheduler);
        exit(EXIT_FAILURE);
    }

    // Add processes to MLFQ scheduler
    add_process(scheduler, p1);
    add_process(scheduler, p2);
    add_process(scheduler, p3);
    add_process(scheduler, p4);

    // Run scheduler until all processes complete
    printf("\nStarting MLFQ Scheduler:\n");
    bool processes_remaining = true;
    while (processes_remaining) {
        processes_remaining = false;
        
        // Check if any queue has processes
        for (int i = 0; i < MAX_QUEUES; i++) {
            if (scheduler->queues[i].count > 0) {
                processes_remaining = true;
                break;
            }
        }
        
        if (processes_remaining) {
            execute_time_slice(scheduler, kernel_stack_info);
        }
    }
    printf("Scheduler finished\n\n");

    // Memory analysis
    printf("BEFORE KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    // Explicitly kill a process for demonstration
    printf("Killing process %d\n", p1->pid);
    kill_p(p1, kernel_stack_info);

    printf("AFTER KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    // Clean up remaining memory blocks (if any)
    if (top != NULL) {
        clean_memory_blocks(kernel_stack_info);
    }

    printf("AFTER CLEANUP:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    // Clean up
    free(scheduler);
    free(kernel_stack_info);
    return EXIT_SUCCESS;
}
