/*
 * sumsq.c
 *
 * CS 446.646 Project 1 (Pthreads)
 *
 * Compile with --std=c99
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// aggregate variables
volatile long sum = 0;
volatile long odd = 0;
volatile long min = INT_MAX;
volatile long max = INT_MIN;
volatile bool done = false;
volatile int num_worker_threads;

// function prototypes
void calculate_square(long number);
//void *idle(void *value);

//defining the node and singly linked-list data type (adapted from https://www.learn-c.org/en/Linked_lists)
//defining the node data structure
typedef struct node {
    char act;
    int val;
    struct node * next;
} node_t;
//Function prototypes for linked list functions
void push(node_t * head, char act, int val);
node_t* pop(node_t ** head);

/*void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}*/

//MAIN
int main(int argc, char* argv[])
{
  // check and parse command line options
  if (argc != 3) {
    //printf("Usage: par_sumsq <infile> <number of threads>\n");
    exit(EXIT_FAILURE);
  }
  
  //Saving number of worker threads from in-line command
  num_worker_threads = strtol(argv[2], NULL, 10);
  //If number of worker threads are non-positive, display error and exit
  if(num_worker_threads < 1){
  	//printf("ERROR: Number of worker threads is not non-positive\n");
  	exit(EXIT_FAILURE);
  }
  
  char *fn = argv[1];
  
  node_t * head = NULL;
  head = (node_t*) malloc(sizeof(node_t));
  if (head == NULL){
  	return 1;
  }
  node_t * popped_node = NULL;
  popped_node = (node_t*) malloc(sizeof(node_t));
  
  // load numbers and add them to the queue
  FILE* fin = fopen(fn, "r");
  char action;
  long num;
  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
  	push(head, action, num);
    
  }
  fclose(fin);
  
  //Creating worker threads
  pthread_t workers[num_worker_threads];
  for(long i = 0; i < num_worker_threads; ++i){
  	pthread_join(workers[i], NULL);
  }
  
  //Reading from the task queue and carrying tasks out accordingly
  popped_node = pop(&head);
  while (popped_node != NULL){
  	char temp_action;
  	char temp_number;
  	pthread_t tid = 0;
  	temp_action = popped_node -> act;
  	temp_number = popped_node -> val;
  	if(temp_action == 'w'){
  		sleep(temp_number);
  	}
  	else if(temp_action == 'p'){
  		pthread_create(&tid, NULL, caculate_square, temp_number);
  		pthread_join(tid, NULL);
  	}
  	else{
  		//printf("ERROR: Unrecognized action: '%c'\n", action);
      		exit(EXIT_FAILURE);
  	}
  	//printf("%c %d\n", temp_action, temp_number);
  	++tid;
  	popped_node = pop(&head);
  }
  
  // print results
  //printf("%ld %ld %ld %ld\n", sum, odd, min, max);
  
  // clean up and return
  return (EXIT_SUCCESS);
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

	//NEED IMPLEMENT MUTEX LOCK TO ENSURE GLOBAL VARIABLES ARE UPDATED CORRECTLY -- when another process is updating, block until free to update
/*// let's add this to our (global) sum
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
  }*/
}

/*
 * Function for pthreads to be idle
 */
/*void *idle(void *value){

}*/

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
 * Function to remove first node of linked list
 */
node_t* pop(node_t ** head) {
    node_t* node_to_return;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return NULL;
    }

    next_node = (*head)->next;
    node_to_return = *head;
    free(*head);
    *head = next_node;

    return node_to_return;
}



