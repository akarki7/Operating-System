#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h> //for the threads

static int P=100;
static long int N=10000;

static char coin_array[]={'O','O','O','O','O','O','O','O','O','O','X','X','X','X','X','X','X','X','X','X','\0'};
int status = EXIT_SUCCESS;

static pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;

void *run_threads(int n, void*(*proc)(void *))
{
    pthread_t tid[P];//we need total P threads i.e P persons
    void * thread_status[P];//status after joining
    int j;
    for(j=0;j<P;j++)
    {
        (void)pthread_create(&tid[j],NULL,proc,NULL);
    }

    // Wait for all threads and join them.
	for (j = 0; j < P; j++) 
    {
		pthread_join(tid[j], &thread_status[j]);
	}
}

static double timeit(int n, void* (*proc)(void *))
{
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double) t2 - (double) t1) / CLOCKS_PER_SEC * 1000;
}


//function for thread strategy 1
void*coin_flip_1()
{
    (void)pthread_mutex_lock(&mutex);
    int i=0;
    int j=0;
    for(j=0;j<N;j++)
    {
        i=0;
        while(coin_array[i]!='\0')
        {
            if(coin_array[i]=='O')
            {
                coin_array[i]='X';
            }
            else
            {
                coin_array[i]='O';
            }
        
            i++;
        }
    }

    (void)pthread_mutex_unlock(&mutex);
}

//function for thread second strategy
void*coin_flip_2()
{
    int i=0;
    int j=0;
    for(j=0;j<N;j++)
    {
        (void)pthread_mutex_lock(&mutex);//lock for iteration
        i=0;
        while(coin_array[i]!='\0')
        {
            if(coin_array[i]=='O')
            {
                coin_array[i]='X';
            }
            else
            {
                coin_array[i]='O';
            }
        
            i++;
        }
        (void)pthread_mutex_unlock(&mutex);
    }
}

//function for thread third strategy
void*coin_flip_3()
{
    int i=0;
    int j=0;
    for(j=0;j<N;j++)
    {
        i=0;
        while(coin_array[i]!='\0')
        {
            (void)pthread_mutex_lock(&mutex);//lock for single coin
            if(coin_array[i]=='O')
            {
                coin_array[i]='X';
            }
            else
            {
                coin_array[i]='O';
            }
            i++;
            (void)pthread_mutex_unlock(&mutex);
        }
    }
}



void print_coins(int v)
{
    int i=0;
    while(coin_array[i]!='\0')
    {
        printf("%c",coin_array[i]);
        i++;
    }
    if (v==1)
    {
        printf(" (start - global lock)\n");
    }
    else if (v==2)
    {
        printf(" (end - global lock)\n");
    }
    else if (v==3)
    {
        printf(" (start - iteration lock)\n");
    }
    else if (v==4)
    {
        printf(" (end - table lock)\n");
    }
    else if (v==5)
    {
        printf(" (start - coin lock)\n");
    }
    else
    {
        printf(" (end - coin lock)\n");
    }
}


int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, ":if:p:n:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            printf("Input for person nubmer given\n");
            P=atoi(optarg);//optarg
            break;
        case 'n':
            printf("Input for total flip nubmer given\n");
            N=atoi(optarg);//optarg
            break;
        case '?':
                printf("Unknown option: %c\n",optopt);
                break;
        }
    }
    for(; optind < argc; optind++)
    {      
        printf("extra arguments: %s\n", argv[optind]);  
    } 

    int j;
    printf("P = %d\n",P);
    printf("N = %ld\n",N);
    printf("\n");

    double t1,t2,t3;

    //first strategy
    print_coins(1);
    t1=timeit(P,coin_flip_1);
    print_coins(2);
    printf("%d threads x %ld flips: %lf ms\n",P,N,t1);

    printf("\n");

    //second strategy
    print_coins(3);
    t2=timeit(P,coin_flip_2);
    print_coins(4);
    printf("%d threads x %ld flips: %lf ms\n",P,N,t2);

    printf("\n");

    //third strategy
    print_coins(5);
    t3=timeit(P,coin_flip_3);
    print_coins(6);
    printf("%d threads x %ld flips: %lf ms\n",P,N,t3);

    return status;
}