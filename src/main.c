#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#define _LIMIT 1000

#define MEM_BLOCK_SIZE sizeof(struct Mem_Block)

struct Mem_Block{
    struct Process* process;
    struct Mem_Block *next_b;
}*top = NULL;
struct Mem_Block* kernel_stack_mem;
static size_t kernel_stack_used = 0;

struct Context{
    int eip;
    int esp;
    int ebc;
    int edx;
    int esi;
    int edi;
    int ebp;
};
enum P_State{
    READY,
    RUNNING,
    SLEEPING,
    UNSED,
};
struct Process{
    unsigned int size;
    enum P_State state;
    int pid;
    int slp : 1;
    int kill: 1;
    struct Context context;
};
void push_P(struct Process * p){
    struct Mem_Block *b = (struct Mem_Block *) malloc(sizeof(struct Mem_Block));
    if (!b){
        fprintf(stderr,"allocation failed\n");
        exit(1);
    }
    if (p->state !=READY){
        fprintf(stderr, "process not ready for execution\n");
        exit(1);
    }
    if((kernel_stack_used + MEM_BLOCK_SIZE + p->size) > (_LIMIT * sizeof(uint64_t))){
        fprintf(stderr, "Memory Full\n");
    }
    kernel_stack_used += (p->size + MEM_BLOCK_SIZE);
    b->process = p;
    b->next_b = NULL;
    b->next_b = top;
    top = b;
}
void run_P(struct Process *p){
    if (p->state != READY){
        fprintf(stderr, "Not ready for execution\n");
    }
   while(p->state = RUNNING){
        printf("process : %d\n", p->pid);
        sleep(1);
    }
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
        exit(1);
    }
    ret = pthread_join(thread, NULL);
    if(ret){
        fprintf(stderr, "Error joining thread : %d\n", ret);
        exit(1);
    }
}
struct Process * creatProcess(int id, size_t psize){

    struct Context context = {12, 13, 3,0,0,0,0};
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    if(!p){
        fprintf(stderr, "memory allocation of process failed\n");
        exit(1);
    }
    p->size = psize;
    p->state = UNSED;
    p->pid = id;
    p->slp = 0;
    p->kill = 0;
    p->context = context;

    return (p);
}
int main(int argc, char *argv[])
{
    struct Mem_Block *kernel_stack_mem = (struct Mem_Block*) malloc(sizeof(struct Mem_Block) * _LIMIT);
    
    struct Process *p1 = NULL;
    p1 = creatProcess(1234, 1000);

    if(!p1){
        fprintf(stderr, "memory allocation of process failed\n");
        free(p1);
    }
    p1->state = READY;
    push_P(p1);
    run_process_thread(p1);


}

