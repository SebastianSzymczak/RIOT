/**
 * @author     	Sebastian Szymczak <sebastian.szymczak@haw-hamburg.de>
 * @ingroup    	examples
 *
 * @file		main.c
 * @brief       Example application for demonstrating RIOT's MQTT-SN library
 *              emCute
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "msg.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"

#define EMCUTE_PORT         (1883U)
#define EMCUTE_ID           ("Rudi_Brudi")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

#define BROKER_IP			"fdaa:bb:cc:ee::3"
#define TOPIC_SUB			"subscriber/light"

static char stack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];
static emcute_sub_t subscriptions[NUMOFSUBS];


static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


static void on_pub(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");
}


int main(void)
{
	/* Gateway-Structure */
	sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
	unsigned i = 0;
	
    puts("MQTT-SN example application\n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");
	
	/*	Creating the Gateway address*/
	if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, BROKER_IP) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }
	
	
    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, (sizeof(queue) / sizeof(msg_t)));
    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));
	
    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");
	printf("Trying to connect\n");	
	
	/* Waiting time for global address can be generated  */
	xtimer_sleep(5);
	/* Function to establish a connection */
	if (emcute_con(&gw, true, NULL, NULL, 0, 0) != EMCUTE_OK) {
	   printf("error: unable to connect to Gateway\n\n\n");
	   return 1;
	}			  
	printf("Successfully connected to Gateway\n");
		  
		  
    subscriptions[i].cb = on_pub;
    subscriptions[i].topic.name = TOPIC_SUB;
	
	/* Sign in to subcribe */
		if (emcute_sub(&subscriptions[i], EMCUTE_QOS_0) != EMCUTE_OK) {
			printf("error: unable to subscribe to Gateway\n");
			return 1;
		}
    printf("Now subscribed to Gateway\n");
	
   
    /* should be never reached */
    return 0;
}