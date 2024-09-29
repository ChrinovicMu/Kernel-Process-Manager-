#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#define _LIMIT 1000

struct Mem_Block{
    struct Process* process;
    struct Node *next_b;
}*top = NULL;
uint64_t kernel_stack_mem = (uint64_t *) malloc(sizeof(uint64_t) * 1000);
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
    char *mem;
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
    if((kernel_stack_used + p->size) > (_LIMIT * sizeof(uint64_t))){
        fprintf(stderr, "Memory Full\n");
    }
    kernel_stack_used += p->size;
    b->process = p;
    b->next_b = NULL;
    b->next_b = top;
    top = b;
}
void run_P(struct Process *p){
    if (p->state != READY){
        fprintf(stderr, "Not ready for execution\n");
    }
    p->state = RUNNING;
    printf("process : %d\n", p->pid);
}
void *thread_func(void *arg){
    struct Process *p = (struct Process)arg;
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
void run_next_b(int pid){
    struct Mem_Block *temp_p;
    temp_p = top;
    while(temp_p->process->pid != pid){
        temp_p = temp_p->next_b;
    }
    if(p->next_b == NULL){
        fprintf(stderr, "No process after"\n);
    }
    temp->state = SLEEPING;
    temp_p = p->next_b;
    run_P(temp_p);
}

int main(int argc, char *argv[])
{

}

