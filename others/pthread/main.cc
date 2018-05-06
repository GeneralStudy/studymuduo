#include <pthread.h>

#include <cstdio>
#include <cstdlib>

void *PrintHello(void* threadId)
{
    int64_t tid;
    tid = reinterpret_cast<int64_t>(threadId);

    printf("hello, it's thread #%d \n", tid);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) 
{
    pthread_t threads[5];

    int rc;
    for (int t = 0; t < 5; t++) {
        printf("in main: create thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PrintHello, (void*)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    //pthread_exit(NULL);
}