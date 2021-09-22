#include <pthread.h>

// This is the worker thread function, it has a detached state
void *worker_thread(void *arg)
{
    // Detach the thread
    pthread_detach(pthread_self());

    // Do some work
    printf("Doing some work\n");

    // Exit the thread
    pthread_exit(NULL);
}
