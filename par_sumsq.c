/*
 * sumsq.c
 *
 * CS 446.646 Project 1 (Pthreads)
 *
 * Compile with --std=c99
 */

#include <pthread.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//defining the node and singly linked-list data type (adapted from https://www.learn-c.org/en/Linked_lists)
// defining the node data structure
typedef struct node {
    char act;
    int val;
    struct node * next;
} node_t;


// aggregate variables
volatile long sum = 0;
volatile long odd = 0;
volatile long min = INT_MAX;
volatile long max = INT_MIN;
volatile bool done = false;
volatile int num_worker_threads;
//volatile int worker_thread_count = 0;
pthread_mutex_t lock;



// function prototypes
void calculate_square(long number);
//function prototypes for the linked list data structure
void push(node_t * head, char act, int val);
node_t * pop(node_t *head);
//void traverseList(node_t * head);

int main (int argc, char* argv[]){

// check and parse command line options
  if (argc != 3) {
    printf("Usage: par_sumsq <infile> <number of threads>\n");
    exit(EXIT_FAILURE);
  }
  
  //Saving number of worker threads from in-line command
  num_worker_threads = strtol(argv[2], NULL, 10);
  //If number of worker threads are non-positive, display error and exit
  if(num_worker_threads < 1){
  	printf("ERROR: Number of worker threads is non-positive\n");
  	exit(EXIT_FAILURE);
  }

 char *fn = argv[1];
  
  node_t * head = NULL;
  head = (node_t*) malloc(sizeof(node_t));
  if (head == NULL){
  	return 1;
  }
  
  // load numbers and add them to the queue
  FILE* fin = fopen(fn, "r");
  char action;
  long num;
  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
  	if(action == 'p' || action == 'w'){
  		push(head, action, num);
    }
    else{
    	printf("ERROR: Unrecognized action: '%c'\n", action);
    	exit(EXIT_FAILURE);
    }
  }
  fclose(fin);
  
  pthread_mutex_init(&lock, NULL);
  
  	node_t * task = NULL;
  	task = head;
  	int value;
  	pthread_t thread_id[num_worker_threads];
	int i = 0;
  while(task != NULL){
  	value = task-> val;
  	if(value){
  		action = task -> act;
  		switch (action){
    			case 'w':	sleep(value);
    						break;
    						
    			case 'p':	pthread_create(&(thread_id[i]), NULL, (void*) calculate_square, (void*) value);
    						if (pthread_join(thread_id[i], NULL) == 1){
    							if (i < num_worker_threads){
    								++i;
    								pthread_create(&(thread_id[i]), NULL, (void*) calculate_square, (void*) value);
    							}
    						}
    						
    						for(int j = i; j >= 0; --j){
    							pthread_join(thread_id[j], NULL);
    						}
    						break;
    		
    	}
  		
  	}
  	task = task -> next;
  	
  }
  
  
  //traverseList(head);//function to traverse through task queue and perform its tasks
  
	// print results
	printf("%ld %ld %ld %ld\n", sum, odd, min, max);
  
	// clean up and return
	return (EXIT_SUCCESS);
}

/*
 * Function to add a node to end of linked list
 */
void push(node_t * head, char act, int val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /*adding new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->act = act;
    current->next->val = val;
    current->next->next = NULL;
}


/*
 * Function to get first node of list
 */
node_t * pop(node_t *head){
	node_t * node_to_return;
	node_to_return = head;
	head = head -> next;
	return node_to_return;
}

/*
 * Function to deallocate node
 */
void deallocate (node_t * node_to_delete){
	free (node_to_delete);
}

/*
 * update global aggregate variables given a number
 */
void calculate_square(long number)
{
  // calculate the square
  long the_square = number * number;

  // ok that was not so hard, but let's pretend it was
  // simulate how hard it is to square this number!
  sleep(number);

	//need to acquire a lock to update the global variables
	pthread_mutex_lock(&lock);
  // let's add this to our (global) sum
  sum += the_square;

  // now we also tabulate some (meaningless) statistics
  if (number % 2 == 1) {
    // how many of our numbers were odd?
    odd++;
  }

  // what was the smallest one we had to deal with?
  if (number < min) {
    min = number;
  }

  // and what was the biggest one?
  if (number > max) {
    max = number;
  }
  
  pthread_mutex_unlock(&lock); //thread releases lock after completion of its task
}
