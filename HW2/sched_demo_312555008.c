// sudo ./sched_demo -n 4 -t 0.5 -s NORMAL,FIFO,NORMAL,FIFO -p -1,10,-1,30
#define _GNU_SOURCE  
#define SCHED_NORMAL 0
#include <stdio.h>
#include <sched.h> 
#include <unistd.h> 
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include<string.h>
pthread_barrier_t barrier;

typedef struct {
    pthread_t thread_id;
    int thread_num;
    int sched_policy;
    int sched_priority;
    double buzy_period;
} thread_info_t;

static double my_clock(void) {
	struct timespec t;
	assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
	return 1e-9 * t.tv_nsec + t.tv_sec;
}

void *thread_func(void *arg)
{
    thread_info_t *my_data  = (thread_info_t*)arg;
    int thread_n = my_data->thread_num;
    double bzp = my_data->buzy_period;

    /* 1. Wait until all threads are ready */
    pthread_barrier_wait(&barrier);

    /* 2. Do the task */ 
    for (int i = 0; i < 3; i++) {
		printf("Thread %d is running\n", thread_n);
        double sttime = my_clock();
            while (1) {
            if (my_clock() - sttime >= bzp)
                break;
            }
    }
    /* 3. Exit the function  */
    return NULL;
}
int main(int argc, char* argv[]) {
    int ch=0;
    int max_thread = 0;
    double buzy_period = 0;
    char policies[1000];
    char priority[1000];
     /* 1. Parse program arguments */
    while ((ch = getopt(argc,argv,"n:t:s:p:"))!=-1)
    {
        switch(ch){
            case 'n':
                max_thread = atoi(optarg);
                break;
            case 't':
                buzy_period = strtod(optarg,NULL);
                break;
            case 's':
                strcpy(policies,optarg);
                break;
            case 'p':
                strcpy(priority,optarg);
                break;
            default: 
                break;
        }
    }

    /* 2. Create <num_threads> worker threads */
    thread_info_t *th;
    th = malloc(max_thread*sizeof(thread_info_t));
    
    char *token;
    int t=0;
    token = strtok(policies, ",");
    while( token != NULL ) {
        if(strcmp("NORMAL",token)==0)
            th[t].sched_policy=0;
        else 
            th[t].sched_policy=1;
        t++;
        token = strtok(NULL, ",");
    }
    t = 0;
    token = strtok(priority, ",");
    while( token != NULL ) {
        th[t].sched_priority = atoi(token);
        th[t].buzy_period = buzy_period;
        t++;
        token = strtok(NULL, ",");
    }

    /* 3. Set CPU affinity */
    int cpu_id = 0; 
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    sched_setaffinity(getpid(), sizeof(cpuset), &cpuset); 


    pthread_barrier_init(&barrier, NULL, max_thread+1);
    pthread_attr_t attr;

    for (int i = 0; i < max_thread; i++) {
        /* 4. Set the attributes to each thread */
        th[i].thread_num=i;
        if(th[i].sched_priority != -1){
            pthread_attr_init(&attr);
            struct sched_param sp ;
            sp.sched_priority = th[i].sched_priority;
            pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
            pthread_attr_setschedparam(&attr, &sp);
            pthread_create(&th[i].thread_id, &attr, thread_func, (void *)(th+i));
        }
        else{
            //default is NORMAL
            // pthread_attr_init(&attr);
            // pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            // pthread_attr_setschedpolicy(&attr, SCHED_BATCH);
            pthread_create(&th[i].thread_id,NULL, thread_func, (void *)(th+i));
        }
    }
    /* 5. Start all threads at once */
    pthread_barrier_wait(&barrier);

    /* 6. Wait for all threads to finish  */ 
    for (int j = 0; j < max_thread; j++)
        pthread_join(th[j].thread_id, NULL); 

    pthread_barrier_destroy(&barrier);
    
    return 0;
}