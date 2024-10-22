#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MEMORY_LIMIT 1024
#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

//process memory space 
struct Mem_Block{
    struct Process* process;
    struct Mem_Block *next_b;
    struct Mem_Block *previous_b;
}*top = NULL;

//infomation on memory 
struct Kernel_Info{
    size_t kernel_stack_mem;
    size_t kernel_stack_used;
};

//process's register usage 
struct Context{
    uint32_t eip;
    int32_t esp;
    int32_t ebc;
    int32_t edx;
    int32_t esi;
    int32_t edi;
    int32_t ebp;
};

//state of process
enum P_State{
    READY,
    RUNNING,
    SLEEPING,
    UNUSED,
    FINISHED,
};

//process structure 
struct Process{
    unsigned int size;      //size of process
    enum P_State state;     //state
    int pid;                //process id 
    int slp : 1;            //ignore 
    int kill: 1;            //ignore 
    struct Context context; // register usage
};
int push_P(struct Process * p, struct Kernel_Info * kernel_stack_info){

    pthread_mutex_lock(&process_mutex);
    struct Mem_Block *b = (struct Mem_Block *) malloc(sizeof(struct Mem_Block));

    if (!b){
        fprintf(stderr,"allocation failed\n");
        pthread_mutex_unlock(&process_mutex);
        return -1; 
    }
    if (p->state !=READY){
        free(b);
        fprintf(stderr, "process not ready for execution\n");
        pthread_mutex_unlock(&process_mutex);
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
    pthread_mutex_unlock(&process_mutex);
    return 0;
}
void run_P(struct Process *p){

    pthread_mutex_lock(&process_mutex);
    if (p->state != READY){
        fprintf(stderr, "Not ready for execution\n");
        pthread_mutex_unlock(&process_mutex);
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
    run_P(p);
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

struct Process * creatProcess(unsigned int id){
    if(id == 0){
        fprintf(stderr, "invalid process id\n");
        return NULL;
    }
    struct Context context = {12, 13, 3,0,0,0,0};    //arbitury test values
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));

    if(!p){
        fprintf(stderr, "memory allocation of process failed\n");
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
void kill_P(struct Process *p, struct Kernel_Info *kernel_stack_info){
    
    pthread_mutex_lock(&process_mutex);

    if(p == NULL || kernel_stack_info == NULL){
        fprintf(stderr, "invalid pointer passed to kill\n");
        pthread_mutex_unlock(&process_mutex);
        return;
    }

    if(p->state != FINISHED){
        fprintf(stderr, "process not on stack\n");
        pthread_mutex_unlock(&process_mutex);
        return;
    }

    struct Mem_Block *current = top;
    struct Mem_Block *prev = NULL;

    while(current != NULL){
        if(current->process->pid == p->pid){
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
    fprintf(stderr, "Process not found in memory blocks\n");
    pthread_mutex_unlock(&process_mutex);
}
void clean_memoryBlocks(){

    struct Mem_Block *current = NULL;
    current = top;
    while (current != NULL){
        struct Mem_Block *next = current->next_b;
        free(current);
        current = next;
    }
}
int main(int argc, char *argv[])
{
    struct Kernel_Info *kernel_stack_info = (struct Kernel_Info*) malloc(sizeof(struct Kernel_Info));
    if (kernel_stack_info == NULL){
        fprintf(stderr,"Error allocating kernel info memory");
        exit(1);
    }
    kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT;
    kernel_stack_info->kernel_stack_used = 0;

    struct Process *p1 = NULL;
    struct Process *p2 = NULL;
    struct Process *p3 = NULL;
    struct Process *p4 = NULL;

    p3 = creatProcess(2222);
    p4 = creatProcess(4444);
    p1 = creatProcess(3333);
    p2 = creatProcess(9999);

    if(!p1 || !p2 || !p3 || !p4){
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(kernel_stack_info);
        exit(1);
    }

    int push1, push2, push3, push4;

    push1 = push_P(p1, kernel_stack_info);
    push2 = push_P(p2, kernel_stack_info);
    push3 = push_P(p3, kernel_stack_info);
    push4 = push_P(p4, kernel_stack_info);

    if(push1 != 0 || push2 != 0 || push3 != 0 || push4 != 0){
        fprintf(stderr, "Couldn't push to the stack\n");
        return -1;
    }
    struct Process *p_array[4] = {p1, p2, p3, p4};
    size_t len = sizeof(p_array)/ sizeof(p_array[0]);

    run_process_threads(p_array, len);

    //JUST FOR ANALYSIS AND TESTING OF Kill_p()
    printf("BEFORE KILL :\n");
    printf("kernel stack memory : %d\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %d\n\n", kernel_stack_info->kernel_stack_used);

    kill_P(p1, kernel_stack_info);
    kill_P(p2, kernel_stack_info);
    kill_P(p3, kernel_stack_info);
    kill_P(p4, kernel_stack_info);
    clean_memoryBlocks();

    printf("AFTER KILL : \n");
    printf("kernel stack memory : %d\n",kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %d\n\n", kernel_stack_info->kernel_stack_used);
    free(kernel_stack_info);

}

