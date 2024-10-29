#include "prolib.h"

#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)

static struct Mem_Block* top = NULL;
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Process* creatProcess(unsigned int id) {
    if(id == 0) {
        fprintf(stderr, "invalid process id\n");
        return NULL;
    }
    struct Context context = {12, 13, 3, 0, 0, 0, 0, 0, 0};
    struct Process* p = (struct Process*)malloc(sizeof(struct Process));

    if(!p) {
        fprintf(stderr, "Process memory allocation failed\n");
        return NULL;
    }
    p->size = sizeof(struct Process);
    p->state = READY;
    p->pid = id;
    p->slp = 0;
    p->kill = 0;
    p->context = context;

    return p;
}

int push_P(struct Process* p, struct Kernel_Info* kernel_stack_info) {
    if (p == NULL || kernel_stack_info == NULL) {
        fprintf(stderr, "invalid pointers\n");
        return -1;
    }

    pthread_mutex_lock(&process_mutex);
    struct Mem_Block* b = (struct Mem_Block*)malloc(sizeof(struct Mem_Block));

    if (!b) {
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "allocation failed\n");
        return -1; 
    }

    if (p->state != READY) {
        free(b);
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "process not ready\n");
        return -1;
    }

    if((kernel_stack_info->kernel_stack_used + MEM_BLOCK_SIZE + p->size) > 
       (kernel_stack_info->kernel_stack_mem)) {
        free(b);
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "Memory Full\n");
        return -1;
    }

    kernel_stack_info->kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
    b->process = p;

    if(top == NULL) {
        top = b;
        b->next_b = NULL;
        b->previous_b = NULL;
    } else {
        b->next_b = top;
        b->previous_b = NULL;
        top->previous_b = b;
        top = b;
    }

    pthread_mutex_unlock(&process_mutex);
    return 0;
}

void run_P(struct Process* p) {
    if(p == NULL) {
        fprintf(stderr, "invalid process pointer\n");
        return;
    }

    pthread_mutex_lock(&process_mutex);
    if (p->state != READY) {
        fprintf(stderr, "Not ready for execution\n");
        pthread_mutex_unlock(&process_mutex);
        return;
    }
    p->state = RUNNING;
    pthread_mutex_unlock(&process_mutex);

    int execution_time = 10;
    for (size_t x = 0; x < execution_time; ++x) {
        printf("process : %d -> RUNNING\n", p->pid);
        sleep(1);
    }

    pthread_mutex_lock(&process_mutex);
    p->state = FINISHED;
    pthread_mutex_unlock(&process_mutex);
    printf("process : %d -> FINISHED\n\n", p->pid);
}

static void* thread_func(void* arg) {
    run_P((struct Process*)arg);
    return NULL;
}

void run_process_threads(struct Process* p_array[], size_t len) {
    pthread_t* thread_arr = malloc(len * sizeof(pthread_t));
    if (!thread_arr) {
        fprintf(stderr, "Failed to allocate thread array\n");
        return;
    }

    for(size_t x = 0; x < len; ++x) {
        if (pthread_create(&thread_arr[x], NULL, thread_func, p_array[x]) != 0) {
            fprintf(stderr, "Error creating thread %zu\n", x);
            free(thread_arr);
            return;
        }
    }

    for(size_t x = 0; x < len; ++x) {
        pthread_join(thread_arr[x], NULL);
    }

    free(thread_arr);
}

void kill_P(struct Process* p, struct Kernel_Info* kernel_stack_info) {
    pthread_mutex_lock(&process_mutex);

    if(p == NULL || kernel_stack_info == NULL) {
        fprintf(stderr, "invalid process ptr\n");
        pthread_mutex_unlock(&process_mutex);
        return;
    }

    struct Mem_Block* current = top;
    struct Mem_Block* prev = NULL;

    while(current != NULL) {
        if(current->process->pid == p->pid) {
            if (prev == NULL) {
                top = current->next_b;
            } else {
                prev->next_b = current->next_b;
            }
            kernel_stack_info->kernel_stack_used -= (MEM_BLOCK_SIZE + p->size);
            free(current->process);
            free(current);
            pthread_mutex_unlock(&process_mutex);
            return;
        }
        prev = current;
        current = current->next_b;
    }

    fprintf(stderr, "Process not in mem block\n");
    pthread_mutex_unlock(&process_mutex);
}

void clean_memoryBlocks(struct Kernel_Info* kernel_stack_info) {
    if(top == NULL) {
        printf("No memory blocks to clear\n");
        return;
    }

    struct Mem_Block* current = top;
    while (current != NULL) {
        struct Mem_Block* next = current->next_b;
        if(current->process != NULL) {
            kernel_stack_info->kernel_stack_used -= (current->process->size);
            free(current->process);
        }
        kernel_stack_info->kernel_stack_used -= MEM_BLOCK_SIZE;
        free(current);
        current = next;
    }
    top = NULL;
    printf("All memory blocks cleared\n");
}
