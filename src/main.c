#include "prolib.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Initialize kernel info
    struct Kernel_Info* kernel_stack_info = malloc(sizeof(struct Kernel_Info));
    if (!kernel_stack_info) {
        fprintf(stderr, "Error kernel info memory allocation\n");
        return EXIT_FAILURE;
    }
    kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT;
    kernel_stack_info->kernel_stack_used = 0;

    // Create processes
    struct Process* processes[4] = {
        creatProcess(2222),
        creatProcess(3333),
        creatProcess(4444),
        creatProcess(9999)
    };

    // Check process creation
    for (int i = 0; i < 4; i++) {
        if (!processes[i]) {
            fprintf(stderr, "Process creation failed\n");
            // Clean up already created processes
            for (int j = 0; j < i; j++) {
                free(processes[j]);
            }
            free(kernel_stack_info);
            return EXIT_FAILURE;
        }
    }

    // Push all processes to memory
    for (int i = 0; i < 4; i++) {
        if (push_P(processes[i], kernel_stack_info) != 0) {
            fprintf(stderr, "Failed to push process to memory\n");
            // Clean up
            for (int j = 0; j < 4; j++) {
                free(processes[j]);
            }
            free(kernel_stack_info);
            return EXIT_FAILURE;
        }
    }

    // Run all processes
    run_process_threads(processes, 4);

    // Memory analysis
    printf("BEFORE KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    // Kill all processes
    for (int i = 0; i < 4; i++) {
        kill_P(processes[i], kernel_stack_info);
    }

    printf("AFTER KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    // Final cleanup
    clean_memoryBlocks(kernel_stack_info);
    printf("AFTER CLEANUP:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    free(kernel_stack_info);
    return EXIT_SUCCESS;
}
