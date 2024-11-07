#include <stdio.h>
#include <stdlib.h>
#include "prolib.h"

int main(int argc, char *argv[])
{
    struct Kernel_Info *kernel_stack_info = (struct Kernel_Info*) malloc(sizeof(struct Kernel_Info));
    if (kernel_stack_info == NULL){
        fprintf(stderr,"Error allocating kernel info memory");
        exit(EXIT_FAILURE);
    }
    kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT; // memory constraint
    kernel_stack_info->kernel_stack_used = 0;

    struct Process *p1 = NULL;
    struct Process *p2 = NULL;
    struct Process *p3 = NULL;
    struct Process *p4 = NULL;

    p3 = create_process(1111);
    p4 = create_process(2222);
    p1 = create_process(3333);
    p2 = create_process(4444);

    if(!p1 || !p2 || !p3 || !p4){
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(kernel_stack_info);
        exit(EXIT_FAILURE);
    }

   //pushes the process on the stack , waiting to be run 
    int push1, push2, push3, push4;

    push1 = push_p(p1, kernel_stack_info);
    push2 = push_p(p2, kernel_stack_info);
    push3 = push_p(p3, kernel_stack_info);
    push4 = push_p(p4, kernel_stack_info);

    if(push1 != 0 || push2 != 0 || push3 != 0 || push4 != 0){
        fprintf(stderr, "Couldn't push to the stack\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(kernel_stack_info);
        exit(EXIT_FAILURE);
    }

    //all processe go into a process array in orde to rin    
    struct Process *p_array[4] = {p1, p2, p3, p4};
    size_t len = sizeof(p_array) / sizeof(p_array[0]);

    //run all process inro the array 
    run_process_threads(p_array, len);

    // Memory analysis
    printf("BEFORE KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    kill_p(p1, kernel_stack_info);
    kill_p(p2, kernel_stack_info);
    kill_p(p3, kernel_stack_info);
    kill_p(p4, kernel_stack_info);

    printf("AFTER KILL:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);
    
    //clears all existing memory blocks 
    clean_memory_blocks(kernel_stack_info);
    printf("AFTER CLEANUP:\n");
    printf("kernel stack memory : %zu\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %zu\n\n", kernel_stack_info->kernel_stack_used);

    free(kernel_stack_info);
    return EXIT_SUCCESS;
}
