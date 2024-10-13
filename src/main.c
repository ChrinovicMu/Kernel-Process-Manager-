#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#define MEMORY_LIMIT 1024

#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

//process memory space 
struct Mem_Block{
    struct Process* process;
    struct Mem_Block *next_b;
}*top = NULL;

//infomation on memory 
struct Kernel_Info{
    size_t kernel_stack_mem;
    size_t kernel_stack_used;
};
//static size_t kernel_stack_mem = ONE_MB;   //overall kernel memory 
//static size_t kernel_stack_used = 0;        //counter of how much memory has been used 

//process's register usage 
struct Context{
    int eip;
    int esp;
    int ebc;
    int edx;
    int esi;
    int edi;
    int ebp;
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

void push_P(struct Process * p, struct Kernel_Info * kernel_stack_info){

    pthread_mutex_unlock(&process_mutex);
    struct Mem_Block *b = (struct Mem_Block *) malloc(sizeof(struct Mem_Block));
    
    if (!b){
        fprintf(stderr,"allocation failed\n");
        pthread_mutex_unlock(&process_mutex);
        return; 
    }
    if (p->state !=READY){
        fprintf(stderr, "process not ready for execution\n");
        pthread_mutex_unlock(&process_mutex);
        return;
    }
    if((kernel_stack_info->kernel_stack_used + MEM_BLOCK_SIZE + p->size) > (kernel_stack_info->kernel_stack_mem)){
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "Memory Full\n");
        return;
    }
    kernel_stack_info->kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
    b->process = p;
    b->next_b = NULL;
    b->next_b = top;
    top = b;
    pthread_mutex_unlock(&process_mutex);
}

void run_P(struct Process *p){

    pthread_mutex_unlock(&process_mutex);
    if (p->state != READY){
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

    pthread_mutex_unlock(&process_mutex);
    p->state = FINISHED;
    printf("process : %d -> FINISHED\n\n", p->pid);
    pthread_mutex_unlock(&process_mutex);
}

void *thread_func(void *arg){
    struct Process *p = (struct Process*)arg;
    pthread_t thread;
    run_P(p);
    return NULL;
}

void run_process_threads(struct Process *p1, struct Process *p2){
    
    pthread_t thread1; 
    pthread_t thread2;
    int t_err;

    t_err = pthread_create(&thread1, NULL, thread_func, (void *)p1);
    if(t_err != 0){
        fprintf(stderr,"Error creating threads\n");
        return;
    }
    t_err = pthread_create(&thread2, NULL, thread_func, (void *)p2);
    if(t_err != 0){
        fprintf(stderr, "Error creating threads\n");
        return;
    }
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&process_mutex);
}

struct Process * creatProcess(int id){

    struct Context context = {12, 13, 3,0,0,0,0};    //arbitury test values
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    if(!p){
        fprintf(stderr, "memory allocation of process failed\n");
        return NULL;
    }
    p->size = sizeof(p);
    p->state = UNUSED;
    p->pid = id;
    p->slp = 0;
    p->kill = 0;
    p->context = context;

    return (p);
}
void kill_P(struct Process *p, struct Kernel_Info *kernel_stack_info){
    if(p->state != FINISHED){
        fprintf(stderr, "Process is not on stack\n");
        return;
    }
    kernel_stack_info->kernel_stack_used = kernel_stack_info->kernel_stack_used - (MEM_BLOCK_SIZE + p->size);
    free(p);
}
int main(int argc, char *argv[])
{
   /* kernel_stack_mem = (struct Mem_Block*) malloc(sizeof(struct Mem_Block) * _LIMIT);
    if (kernel_stack_mem == NULL){
        fprintf(stderr, "failed to allocate kernel memory\n");
        exit(1);
    }*/
    struct Kernel_Info *kernel_stack_info = (struct Kernel_Info*) malloc(sizeof(struct Kernel_Info));
    if (kernel_stack_info == NULL){
        fprintf(stderr,"Error allocating kernel info memory");
        exit(1);
    }
    kernel_stack_info->kernel_stack_mem = MEMORY_LIMIT;
    kernel_stack_info->kernel_stack_used = 0;
    struct Process *p1 = NULL;
    struct Process *p2 = NULL;

    p1 = creatProcess(3333);
    p2 = creatProcess(9999);
    if(!p1 || !p2){
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
        free(p2);
        return -1;
    }
    p1->state = READY;
    p2->state = READY;

    push_P(p1, kernel_stack_info);
    push_P(p2, kernel_stack_info);

    struct Process *p_Array[] = {p1, p2};
    size_t len = sizeof(p_Array) / sizeof(p_Array[0]);

    run_process_threads(p1, p2);

    //JUST FOR ANALYSIS AND TESTING OF Kill_p()
    printf("BEFORE KILL :\n");
    printf("kernel stack memory : %d\n", kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %d\n\n", kernel_stack_info->kernel_stack_used);

    kill_P(p1, kernel_stack_info);
    kill_P(p2, kernel_stack_info);

    printf("AFTER KILL : \n");
    printf("kernel stack memory : %d\n",kernel_stack_info->kernel_stack_mem);
    printf("kernel memory used : %d\n\n", kernel_stack_info->kernel_stack_used);


}

