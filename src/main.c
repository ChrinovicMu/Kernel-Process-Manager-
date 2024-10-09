#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#define _LIMIT 1000

#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

//process memory space 
struct Mem_Block{
    struct Process* process;
    struct Mem_Block *next_b;
}*top = NULL;

struct Mem_Block* kernel_stack_mem;     //overall kernel memory 
static size_t kernel_stack_used = 0;    //counter of how much memory has been used 

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

void push_P(struct Process * p){

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
    if((kernel_stack_used + MEM_BLOCK_SIZE + p->size) > (_LIMIT * sizeof(uint64_t))){
        pthread_mutex_unlock(&process_mutex);
        fprintf(stderr, "Memory Full\n");
        return;
    }
    kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
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
        sleep(2);
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

void run_process_thread(struct Process *p){

    pthread_t thread;
    int ret;

    ret = pthread_create(&thread, NULL, thread_func, (void *) p);
    if(ret){
        fprintf(stderr, "Error creating thread : %d\n", ret);;
        return;
    }
    ret = pthread_join(thread, NULL);
    if(ret){
        fprintf(stderr, "Error joining thread : %d\n", ret);
        return;
    }
    pthread_detach(thread);
}

struct Process * creatProcess(int id, size_t psize){

    struct Context context = {12, 13, 3,0,0,0,0};    //arbitury test values
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    if(!p){
        fprintf(stderr, "memory allocation of process failed\n");
        return NULL;
    }
    p->size = psize;
    p->state = UNUSED;
    p->pid = id;
    p->slp = 0;
    p->kill = 0;
    p->context = context;

    return (p);
}
void kill_P(struct Process *p){
    //TODO: kills process and free up memory
}
int main(int argc, char *argv[])
{
    kernel_stack_mem = (struct Mem_Block*) malloc(sizeof(struct Mem_Block) * _LIMIT);
    if (kernel_stack_mem == NULL){
        fprintf(stderr, "failed to allocate kernel memory\n");
        exit(1);
    }
    struct Process *p1 = NULL;
    struct Process *p2 = NULL;

    p1 = creatProcess(1234, 1000);
    p2 = creatProcess(9876, 1000);
    if(!p1 || !p2){
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
        free(p2);
        free(kernel_stack_mem);
        return -1;
    }
    p1->state = READY;
    p2->state = READY;

    push_P(p1);
    push_P(p2);
    run_process_thread(p1);
    run_process_thread(p2);
    free(kernel_stack_mem);
}

