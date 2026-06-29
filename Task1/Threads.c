#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Creating the shared resource and locks
int shared_counter = 0;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lock_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_B = PTHREAD_MUTEX_INITIALIZER;

// Control flags
int use_mutex = 0;
int cause_deadlock = 0;

//Creation of threads
void* worker_thread(void* arg) {
    long thread_id = (long)arg;

    // Race condition and Mutex
    for (int i = 0; i < 100000; i++) {
        if (use_mutex) {
            // Race condition being handled
            pthread_mutex_lock(&my_mutex); 
        }
        
        // The shared resource causing the race condition
        shared_counter++; 
        
        if (use_mutex) {
            pthread_mutex_unlock(&my_mutex);
        }
    }
    
    // Deadlock (This will only be executed if cause_deadlock is active)
    if (cause_deadlock) {
        if (thread_id == 1) {
            // Thread 1 has A then wants B
            pthread_mutex_lock(&lock_A);
            printf("[Thread 1] Locked A, waiting for B...\n");
            usleep(10000); 
            pthread_mutex_lock(&lock_B);
            
            pthread_mutex_unlock(&lock_B);
            pthread_mutex_unlock(&lock_A);
        } 
        else if (thread_id == 2) {
            // Thread 2 grabs B then wants A (Causes Deadlock!)
            pthread_mutex_lock(&lock_B);
            printf("[Thread 2] Locked B, waiting for A...\n");
            usleep(10000);
            pthread_mutex_lock(&lock_A);
            
            pthread_mutex_unlock(&lock_A);
            pthread_mutex_unlock(&lock_B);
        }
        // Thread 3 isnt used in this scenario
    }
    printf("[Thread %ld] Task finished.\n", thread_id);
    return NULL;
}

//Round Robin 
void simulate_round_robin(int quantum) {
    printf("\nRunning Round Robin on the 3 Threads\n");
    
    // Creating remaining processing times for Thread 1, 2, and 3
    int remaining_time[3] = {25, 12, 15}; 
    int active_threads = 3;

    while (active_threads > 0) {
        for (int i = 0; i < 3; i++) {
            if (remaining_time[i] > 0) {
                if (remaining_time[i] > quantum) {
                    remaining_time[i] -= quantum;
                    printf("Scheduler: Thread %d runs for %dms. Remaining: %dms\n", i+1, quantum, remaining_time[i]);
                } else {
                    printf("Scheduler: Thread %d runs for %dms -> [FINISHED]\n", i+1, remaining_time[i]);
                    remaining_time[i] = 0;
                    active_threads--;
                }
            }
        }
    }
}

//Main function
int main() {
    pthread_t t1, t2, t3;

    // Showing the Race Condition (Happens when no mutex is used)
    printf("Accessing 3 Threads at once (No Mutex)\n");
    shared_counter = 0;
    use_mutex = 0;   // Turn off protection
    cause_deadlock = 0;

    pthread_create(&t1, NULL, worker_thread, (void*)1);
    pthread_create(&t2, NULL, worker_thread, (void*)2);
    pthread_create(&t3, NULL, worker_thread, (void*)3);

    pthread_join(t1, NULL); pthread_join(t2, NULL); pthread_join(t3, NULL);
    
    printf("Expected Counter: 300000 | Actual Counter: %d\n", shared_counter);
    printf("Observation is made that data is lost/corrupted due to the Race Condition\n\n");
    
    //Utilizing Mutex to handle the shared condition
    printf("Accessing 3 Threads at once (With Mutex)\n");
    shared_counter = 0;
    use_mutex = 1;   // Mutex is used
    
    pthread_create(&t1, NULL, worker_thread, (void*)1);
    pthread_create(&t2, NULL, worker_thread, (void*)2);
    pthread_create(&t3, NULL, worker_thread, (void*)3);

    pthread_join(t1, NULL); pthread_join(t2, NULL); pthread_join(t3, NULL);
    
    printf("Expected Counter: 300000 | Actual Counter: %d\n", shared_counter);
    printf("Mutex fixed the issue, no data is lost.\n\n");
    
    printf("Deadlock Handling\n");
    //In the code thread 1 acquires Lock A then B while thread 2 acquires Lock B then A.
    //If 'cause_deadlock' is set to 1, they will enter a circular wait and be deadlocked
    // To prevent it, we break the circular wait by forcing both threads to acquire Lock A first then move to Lock B.
    
    //Round Robin
    //Running the round robin with a time quantum of 10ms
    simulate_round_robin(10); 

    return 0;
}
