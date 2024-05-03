/*
 * Copyright (c) 2015, SICS Swedish ICT.
 * Copyright (c) 2018, University of Bristol - http://www.bristol.ac.uk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \file
 *         A RPL+TSCH node demonstrating application-level time syncrhonization.
 *
 * \author Atis Elsts <atis.elsts@bristol.ac.uk>
 *         Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"

#include "dev/cc26xx-uart.h"
#include "dev/serial-line.h"

#include "sys/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "dev/cc26xx-uart.h"
#include "dev/serial-line.h"
#include "sys/log.h"
#include <stdlib.h>
#include <string.h>

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SEND_INTERVAL (1 * CLOCK_SECOND)

/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&node_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#define MAX_SIZE 20

// Define the structure of a queue node
typedef struct {
    char data[100];
} QueueNode;

// Define the structure of the queue
typedef struct {
    QueueNode items[MAX_SIZE];
    int front, rear;
} Queue;

// Function prototypes
Queue* createQueue();
int isFull(Queue* queue);
int isEmpty(Queue* queue);
void enqueue(Queue* queue, char* data);
char* dequeue(Queue* queue);
char* getFront(Queue* queue);
void display(Queue* queue);
void destroyQueue(Queue* queue);

// Initialize Queue
Queue* queue;

//test int
int length = 0;
const char* message = NULL;
int bytes = 7000;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    //char * serial_msg_recv = NULL;

    // Initialize the queue
    queue = createQueue();

    /* Initialize UART */
    cc26xx_uart_init();
    cc26xx_uart_set_input(serial_line_input_byte);
    //cc26xx_uart_set_input(serial_line_input_byte);


    etimer_set(&periodic_timer, SEND_INTERVAL);




    while (1) {
        etimer_set(&periodic_timer, SEND_INTERVAL);


        //First send
        for(int i = 0; i < bytes; i++) {
            cc26xx_uart_write_byte(102);
        }

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));



/*
        char str1[40] = "s: "; // Make sure it's large enough to hold your string
        char str2[30];
        char str3[5] = " c: ";
        char str4[30];

        sprintf(str2, "%d", b);
        sprintf(str4, "%d", b);
        b++;

        strcat(str1, str2); // Append str2 to str1
        strcat(str1, str3);
        strcat(str1, str4);
        enqueue(queue, str1);


        // Define the string to be sent
        message = getFront(queue);
        length = strlen(message);

        // Transmit each byte of the string over UART

            for (i = 0; i < length; i++) {
                cc26xx_uart_write_byte(message[i]);
            }
            i = 0;
            message = NULL;
            length = 0;
            dequeue(queue);


        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
        */
    }

    PROCESS_END();
}




// Function definitions for queue operations
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

int isFull(Queue* queue) {
    return ((queue->rear + 1) % MAX_SIZE == queue->front);
}

int isEmpty(Queue* queue) {
    return (queue->front == -1);
}

void enqueue(Queue* queue, char* data) {
    if (isFull(queue)) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (isEmpty(queue)) {
        queue->front = 0; // Initialize front pointer if empty
    }
    queue->rear = (queue->rear + 1) % MAX_SIZE; // Circular increment of rear pointer
    strcpy(queue->items[queue->rear].data, data);
}

char* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }
    char* data = queue->items[queue->front].data;
    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_SIZE; // Circular increment of front pointer
    }
    return data;
}

char* getFront(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot get front element.\n");
        return NULL;
    }
    return queue->items[queue->front].data;
}

void display(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue elements: ");
    int i = queue->front;
    do {
        printf("%s ", queue->items[i].data);
        i = (i + 1) % MAX_SIZE; // Circular increment of index
    } while (i != (queue->rear + 1) % MAX_SIZE); // Check if reached rear

    printf("\n");
}

void destroyQueue(Queue* queue) {
    free(queue);
}


