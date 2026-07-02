#include <stdio.h>
#include <stdlib.h>

#define FRAME_SIZE 4

//Creating the empty frames(4 bytes)
int fifo_buffer[FRAME_SIZE];
int lru_buffer[FRAME_SIZE];

//FIFO page replacement algorithm
void simulate_fifo(int pages[], int total_pages) {
    printf("Running FIFO Page Replacement Simulation\n");
    
    //Empty slots for buffer
    for (int i = 0; i < FRAME_SIZE; i++) {
        fifo_buffer[i] = -1; 
    }
    
    int index_to_replace = 0;
    int page_faults = 0;

    for (int i = 0; i < total_pages; i++) {
        int page = pages[i];
        int found = 0;

        //Checking if the page is already in a frame
        for (int j = 0; j < FRAME_SIZE; j++) {
            if (fifo_buffer[j] == page) {
                found = 1;
                break;
            }
        }

        //If the page is not found its a page fault so we replace data
        if (!found) {
            page_faults++;
            
            //With use of index we place data in empty spaces
            fifo_buffer[index_to_replace] = page;
            
            printf("Page %d -> FAULT -> Buffer: [ ", page);
            for (int k = 0; k < FRAME_SIZE; k++) {
                if (fifo_buffer[k] == -1) printf("- ");
                else printf("%d ", fifo_buffer[k]);
            }
            printf("]\n");

            //FIFO rule applied
            index_to_replace = (index_to_replace + 1) % FRAME_SIZE;
        } else {
            printf("Page %d -> HIT   -> Buffer: [ ", page);
            for (int k = 0; k < FRAME_SIZE; k++) {
                if (fifo_buffer[k] == -1) printf("- ");
                else printf("%d ", fifo_buffer[k]);
            }
            printf("]\n");
        }
    }
    printf("Total FIFO Page Faults: %d\n\n", page_faults);
}

//LRU page replacement algorithm
void simulate_lru(int pages[], int total_pages) {
    printf("Running LRU Page Replacement Simulation\n");
    
    //Empty slots for buffer
    for (int i = 0; i < FRAME_SIZE; i++) {
        lru_buffer[i] = -1; 
    }
    
    //Making some counter arrays to keep track of what has been recently used
    int last_used_time[FRAME_SIZE] = {0};
    int current_time = 0;
    int page_faults = 0;

    for (int i = 0; i < total_pages; i++) {
        int page = pages[i];
        int found = -1;
        current_time++;

        //Checking if page already exists in a frame
        for (int j = 0; j < FRAME_SIZE; j++) {
            if (lru_buffer[j] == page) {
                found = j;
                break;
            }
        }

        //On a page hit we update the counter array to keep track of whats been recently used
        if (found != -1) {
            last_used_time[found] = current_time;
            printf("Page %d -> HIT   -> Buffer: [ ", page);
            for (int k = 0; k < FRAME_SIZE; k++) {
                if (lru_buffer[k] == -1) printf("- ");
                else printf("%d ", lru_buffer[k]);
            }
            printf("]\n");
        } else {
            page_faults++;
            int target_index = 0;
            int empty_found = 0;

            //Checking for empty frames first
            for (int j = 0; j < FRAME_SIZE; j++) {
                if (lru_buffer[j] == -1) {
                    target_index = j;
                    empty_found = 1;
                    break;
                }
            }

            //When full we find the Least Recently Used frame and its index
            if (!empty_found) {
                int oldest_time = last_used_time[0];
                target_index = 0;
                for (int j = 1; j < FRAME_SIZE; j++) {
                    if (last_used_time[j] < oldest_time) {
                        oldest_time = last_used_time[j];
                        target_index = j;
                    }
                }
            }

            //Using indexing to place data in LRU frame
            lru_buffer[target_index] = page;
            last_used_time[target_index] = current_time;
            
            printf("Page %d -> FAULT -S> Buffer: [ ", page);
            for (int k = 0; k < FRAME_SIZE; k++) {
                if (lru_buffer[k] == -1) printf("- ");
                else printf("%d ", lru_buffer[k]);
            }
            printf("]\n");
        }
    }

    printf("Total LRU Page Faults: %d\n", page_faults);
}

//Main function
int main() {
    //Array with strings to simulate page requests
    int page_requests[] = {1, 2, 3, 4, 5, 2, 1, 6, 4, 1, 2, 4, 3, 5};
    int total_requests = sizeof(page_requests) / sizeof(page_requests[0]);

    //Simulating FIFO then LRU
    simulate_fifo(page_requests, total_requests);
    simulate_lru(page_requests, total_requests);

    return 0;
}
