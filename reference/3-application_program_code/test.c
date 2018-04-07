#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <semaphore.h>  
  
#define SIZE 1024  
  
void *thread_function(void *arg);  
  
char buffer[SIZE];  
sem_t sem;  
  
int main()  
{  
    int res;  
    pthread_t a_thread;  
    void *thread_result;  
  
    res = sem_init(&sem, 0, 0);  
    if (res != 0)  
    {  
        perror("Sem init failed");  
        exit(EXIT_FAILURE);  
    }  
  
    res = pthread_create(&a_thread, NULL, thread_function, NULL);  
    if (res != 0)  
    {  
        perror("Thread create failed");  
        exit(EXIT_FAILURE);  
    }  
  
    printf("Input some text. Enter 'end' to finish\n");  
  
    while (scanf("%s", buffer))  
    {  
        sem_post(&sem);  
        if (strncmp("end", buffer, 3) == 0)  
            break;  
    }  
  
    printf ("\nWaiting for thread to finish...\n");  
  
    res = pthread_join(a_thread, &thread_result);  
    if (res != 0)  
    {  
        perror("Thread join failed");  
        exit(EXIT_FAILURE);  
    }  
  
    printf ("Thread join\n");  
  
    sem_destroy(&sem);  
  
    exit(EXIT_SUCCESS);  
}  
  
void *thread_function(void *arg)  
{  
    sem_wait(&sem);  
    while (strncmp("end", buffer, 3) != 0)  
    {  
        printf("You input %d characters\n", strlen(buffer));  
        sem_wait(&sem);  
    }  
    pthread_exit(NULL);  
}     