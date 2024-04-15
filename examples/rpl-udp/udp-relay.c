#include <inttypes.h>
#include <stdint.h>

#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "random.h"
#include "examples/rpl-udp/rpl_relay.h"
// extensions
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 2222
#define UDP_SERVER_PORT 1111

#define SEND_INTERVAL (10 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;
static int16_t dummy=0;
int16_t server_RSSI= 1;
int16_t client_RSSI=1;
/*---------------------------------------------------------------------------*/
/*----------------------------------UART-------------------------------------*/
#include "dev/cc26xx-uart.h"
#include "dev/serial-line.h"
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 5

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
        //printf("Queue is empty. Cannot get front element.\n");
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



/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen) {
    LOG_INFO_6ADDR(sender_addr);
    char str1[40] = "s: "; // Make sure it's large enough to hold your string
    char str2[30];
    char str3[5] = " c: ";
    char str4[30];
    char str5[5] = "\n";
#if LLSEC802154_CONF_ENABLED
    LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
    server_RSSI= (int16_t)uipbuf_get_attr(UIPBUF_ATTR_RSSI);
    sprintf(str2, "%d", server_RSSI);
    if(*data==1){
        client_RSSI= *data;
        sprintf(str4, "%d", client_RSSI);
        LOG_INFO_("\n");
    }
    else{
        client_RSSI=*data| -256;
    }
    strcat(str1, str2); // Append str2 to str1
    strcat(str1, str3);
    strcat(str1, str4);
    strcat(str1, str5);
    enqueue(queue, str1);

    rx_count++;
}
int i = 0;
int length = 0;
const char* message = NULL;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data) {
    static struct etimer periodic_timer;
    uip_ipaddr_t dest_ipaddr;
    static uint32_t tx_count;
    static uint32_t missed_tx_count;

    PROCESS_BEGIN();
    /* Initialize the queue */
    queue = createQueue();

    /* Initialize UART */
    cc26xx_uart_init();
    cc26xx_uart_set_input(serial_line_input_byte);

    /* Set the transmission power level to -12 dBm */
    radio_value_t power_level;
    NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &power_level);
    radio_value_t new_power_level = power_level + (radio_value_t) RADIO_OFFSET;
    NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, new_power_level);
    /* Initialize UDP connection */
    simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                        UDP_SERVER_PORT, udp_rx_callback);

    etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));


        //display(queue);
        //printf("First element: %s\n", getFront(queue));

        // Define the string to be sent
        message = getFront(queue);
        length = strlen(message);

        // Transmit each byte of the string over UART
        if (isEmpty(queue)) {

        } else {
            //printf("%s \n", message);
            for (i = 0; i < length; i++) {
                cc26xx_uart_write_byte(message[i]);
            }
            i = 0;
            message = NULL;
            length = 0;
            dequeue(queue);
        }





        if (NETSTACK_ROUTING.node_is_reachable() &&
            NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
            /* Print statistics every 10th TX */
            if (tx_count % 1 == 0) {
                LOG_INFO(" (relay) Tx/Rx/MissedTx: %" PRIu32 "/%" PRIu32 "/%" PRIu32 "\n",
                         tx_count, rx_count, missed_tx_count);
                LOG_INFO("(relay) Server RSSI: %i ; Client RSSI: %i\n",server_RSSI,client_RSSI );
            }

            /* Send to DAG root */ LOG_INFO("(relay) Sending request %" PRIu32 " to \n", tx_count);
            LOG_INFO_6ADDR(&dest_ipaddr);
            simple_udp_sendto(&udp_conn, &dummy, sizeof(dummy), &dest_ipaddr);
            tx_count++;
        } else {
            LOG_INFO("(relay) Not reachable yet\n");
            if (tx_count > 0) {
                missed_tx_count++;
            }
        }

        /* Add some jitter */
        etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
