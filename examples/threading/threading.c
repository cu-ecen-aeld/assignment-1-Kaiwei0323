#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)


void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    //wait 
    if (usleep(1000*(thread_func_args->wait_to_obtain_ms)) == -1)
    {
      printf("usleep failed\n");
      pthread_exit(thread_func_args);
    }  
    
    //obtain mutex
    if(pthread_mutex_lock(thread_func_args->mutex) != 0)
    {
      printf("pthread_mutex_lock failed\n");
      pthread_exit(thread_func_args);   
    }
     
    //wait 
    if (usleep(1000*(thread_func_args->wait_to_release_ms)) == -1)
    {
      printf("usleep failed\n");
      pthread_mutex_unlock(thread_func_args->mutex);
      pthread_exit(thread_func_args);
    }     
    
    //release mutex
    if(pthread_mutex_unlock(thread_func_args->mutex) != 0 )
    {
      printf("pthread_mutex_unlock failed\n");
      pthread_exit(thread_func_args);    
    }   
    
    thread_func_args->thread_complete_success = true;

    pthread_exit(thread_func_args);

}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
     if( thread == NULL || mutex == NULL)
    {
      return false;
    } 
    
    //allocate memory
    struct thread_data *tdata = (struct thread_data*) malloc(sizeof(struct thread_data));
    
    //init mutex
    if (tdata == NULL) { 
        printf("memory allocation failed\n"); 
        return false; 
    } 
   
    //populate structure
    tdata->wait_to_obtain_ms = wait_to_obtain_ms;
    tdata->wait_to_release_ms = wait_to_release_ms;
    tdata->mutex = mutex;
    tdata->thread_complete_success = false;
    
    if (pthread_create(thread, NULL, threadfunc, tdata) != 0)
    {
        printf("pthread create failed\n"); 
        free(tdata);
        tdata = NULL;
        return false;    
    }

    return true;
}

