#include "prolib.h"

pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;
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

    pthread_mutex_lock(&process_mutex);
    if (p->state !=READY){
        free(b);
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "process not ready for execution\n");
        return -1;
    }

    if((kernel_stack_info->kernel_stack_used + MEM_BLOCK_SIZE + p->size) > (kernel_stack_info->kernel_stack_mem)){
        free(b);
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "Memory Full\n");
        return -1;
    }
    kernel_stack_info->kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
    b->process = p;

    add_to_stack(b);
    pthread_mutex_unlock(&process_mutex);
    return 0;
}

void run_p(struct Process *p){

    if(p == NULL){
        fprintf(stderr, "invlid process pointer\n");
        return;
    }
    pthread_mutex_lock(&process_mutex);
    if (p->state != READY){
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "Not ready for execution\n");
        return;
    }
    p->state = RUNNING;
    pthread_mutex_unlock(&process_mutex);

    int execution_time = 10;
    size_t x;

    for (x = 0; x < execution_time; ++x){
        printf("process : %d -> RUNNING\n", p->pid);
        sleep(1);
    }
    pthread_mutex_lock(&process_mutex);
    p->state = FINISHED;
    pthread_mutex_unlock(&process_mutex);

    printf("process : %d -> FINISHED\n\n", p->pid);
}

void *thread_func(void *arg){

    struct Process *p = (struct Process*)arg;
    pthread_t thread;
    run_p(p);
    return NULL;
}

void run_process_threads(struct Process *p_array[], size_t len){
    pthread_t thread_arr[len];
    int x, t_err; 

    for(x = 0; x < len; ++x){
       t_err = pthread_create(&thread_arr[x], NULL, thread_func, p_array[x]);
        if(t_err != 0){
            fprintf(stderr, "Error creating threads\n");
            return; 
        }
    }
    for(x = 0; x < len; ++x){
        t_err = pthread_join(thread_arr[x], NULL);
        if(t_err != 0){
            fprintf(stderr, "Error joinning threads\n");
            return;
        }
    }
}

struct Process * create_process(unsigned int id){
    if(id == 0){
        fprintf(stderr, "invalid process id\n");
        return NULL;
    }
    struct Context context = {0};    //arbitury test values
    context.eip = 12;
    context.esp = 13;
    context.eax = 54;
    context.ebx = 5;

    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    if(!p){
        fprintf(stderr, "Process memory allocation failed\n");
        return NULL;
    }
    p->size = sizeof(struct Process);
    p->state = READY;
    p->pid = id;
    p->slp = 0;
    p->kill = 0;
    p->context = context;
    return (p);
}

void kill_p(struct Process *p, struct Kernel_Info *kernel_stack_info){ 
    if(p == NULL || kernel_stack_info == NULL){
        fprintf(stderr, "invalid process ptr\n");
        return;
    }

    struct Mem_Block *current = top;
    struct Mem_Block *prev = NULL;

    pthread_mutex_lock(&process_mutex);

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
            pthread_mutex_unlock(&process_mutex);
            return;
        }
        prev = current;
        current = current->next_b;
    }
    fprintf(stderr, "Process not in mem block\n");
    pthread_mutex_unlock(&process_mutex);
}

void clean_memory_blocks(){
    struct Mem_Block *current = NULL;
    current = top;
    while (current != NULL){
        struct Mem_Block *next = current->next_b;
        free(current->process);
        free(current);
        current = next;
    }
    top = NULL;
    printf("All memory blocks cleared\n");
}

void rr_schedule(){
    //implementation of round robin scheduling alogorithms 
}
