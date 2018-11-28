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
#include "periph/gpio.h"

#define EMCUTE_PORT         (1883U)
#define EMCUTE_ID           ("Rudi_Brudi")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

#define BROKER_IP			"fdaa:bb:cc:ee::3"
#define TOPIC_SUB_LIGHT_1	"subscriber/light_1"
#define TOPIC_SUB_LIGHT_2	"subscriber/light_2"

static char stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];


static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


static void on_pub_light_1(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf(" got publication for topic '%s'\n", topic->name);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
	puts("");
	
	/* Light 1 turn on and Light 2 turn off */
	gpio_set(GPIO_PIN(0, 23));
	gpio_clear(GPIO_PIN(0, 28));
}


static void on_pub_light_2(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf("got publication for topic '%s' \n", topic->name);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
	puts("");
	
	/* Light 2 turn on and Light 1 turn off */
	gpio_set(GPIO_PIN(0, 28));
	gpio_clear(GPIO_PIN(0, 23));
}


int main(void)
{
	/* Gateway-Structure */
	sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
	
    puts("MQTT-SN example application\n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");
	
	/*	Creating address*/
	if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, BROKER_IP) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }
	
	// LED 1 initialize to OUTPUT
    if (gpio_init(GPIO_PIN(0, 23),  GPIO_OUT) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 23);
        return 1;
    }
	
	// LED 2 OUTPUT
	if (gpio_init(GPIO_PIN(0, 28),  GPIO_OUT) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 28);
        return 1;
    }
	
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
		  
	/*	*/	  
	subscriptions[0].cb = on_pub_light_1;
	subscriptions[0].topic.name = TOPIC_SUB_LIGHT_1;
	/* Sign in to subscribe */
	if (emcute_sub(&subscriptions[0], EMCUTE_QOS_0) != EMCUTE_OK) {
		printf("error: unable to subscribe to Gateway light 1\n");
		return 1;
	}
    printf("Now subscribed to Gateway light 1\n");
	
	/*	*/
	subscriptions[1].cb = on_pub_light_2;
    subscriptions[1].topic.name = TOPIC_SUB_LIGHT_2;
	/* Sign in to subscribe */
	if (emcute_sub(&subscriptions[1], EMCUTE_QOS_0) != EMCUTE_OK) {
		printf("error: unable to subscribe to Gateway light 2\n");
		return 1;
	}
    printf("Now subscribed to Gateway light 2\n");
   
    /* should be never reached */
    return 0;
}
