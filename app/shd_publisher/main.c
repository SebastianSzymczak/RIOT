/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the U8g2 package.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#define DEBUG_FLAG			(0U)
#define TOPIC_SUB_LIGHT1			"subscriber/light_1"
#define TOPIC_SUB_LIGHT2			"subscriber/light_2"
 
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"
#include "u8g2.h"
#include <string.h>
#include "benchmark.h"
#include "irq.h"
#include "led.h"
#include <stdlib.h>
#include "timex.h"
#include "periph/adc.h"
#include "thread.h"

//part of emcute
#include "msg.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"

#define EMCUTE_PORT         (1883U)
#define EMCUTE_ID           ("publisher")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

#define BROKER_IP           ("fdaa:bb:cc:ee::3")
#define BROKER_PORT           ("1883")
// END of EMCUTE

#define RES             ADC_RES_10BIT
#define DELAY (100LU * US_PER_MS)  

//emcute static data

static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];
//static char topics[NUMOFSUBS][TOPIC_MAXLEN];
//
char publisherStack[THREAD_STACKSIZE_MAIN];
char mainStack[THREAD_STACKSIZE_MAIN];


static const char MICRO_CONTROLLER_1[] = "MICRO_CONTROLLER_1";

// emcute part
static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


static int publisherHandler(int argc, char **argv)
{
    emcute_topic_t t;

    /* step 1: get topic id */
    t.name = argv[1];
    if (emcute_reg(&t) != EMCUTE_OK) {
        if(DEBUG_FLAG) puts("error: unable to obtain topic ID");
        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, argv[2], strlen(argv[2]), EMCUTE_QOS_0) != EMCUTE_OK) {
        if(DEBUG_FLAG) printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }

    if(DEBUG_FLAG) printf("Published %i bytes to topic '%s [%i]'\n",
            (int)strlen(argv[2]), t.name, t.id);

			(void)(argc);
    return 0;
}

//
//void *thread_handler(void *arg);

static int raspberry_pi(int sensor1 , int sensor2){
	


	            // send the message to the controller mc as raspberry_pi(int sensor1 , int sensor2)
                if(sensor1 && sensor2){
					  gpio_set(GPIO_PIN(0,23));
					  gpio_set(GPIO_PIN(0,28));
					  char *message[] = {"pub","publisher/bothOn","1"};
						 publisherHandler(3, message);
				  }
				// Reading from the infrared sensor  1
				   else if (sensor1) {
		                 if(DEBUG_FLAG) printf("Movement has been detected in room 1\n");
				         gpio_set(GPIO_PIN(0,23));
						 gpio_clear(GPIO_PIN(0,28));
						 	char *message[] = {"pub",TOPIC_SUB_LIGHT1,"this is sensor 1"};
						 publisherHandler(3, message);
                           }
				// Reading from the infrared sensor  2  
		            else if (sensor2) {
		                 if(DEBUG_FLAG) printf("Movement has been detected in room 2\n");
						 gpio_set(GPIO_PIN(0,28));
				         gpio_clear(GPIO_PIN(0,23));
						 char *message[] = {"pub",TOPIC_SUB_LIGHT2,"this is sensor 2"};
						 publisherHandler(3, message);
                           }
						   // xtimer_sleep(2);	
						   return 0;
};

void *adc(void *argv){

    printf("%s is ON \n" , MICRO_CONTROLLER_1);
    xtimer_ticks32_t last = xtimer_now();
    int sample = 0;

    if(DEBUG_FLAG) puts("\nRIOT ADC peripheral driver test\n");
    if(DEBUG_FLAG) puts("This test will sample all available ADC lines once every 100ms with\n"
         "a 10-bit resolution and print the sampled results to STDIO\n\n");
 
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            if(DEBUG_FLAG) printf("Initialization of ADC_LINE(%u) failed\n", i);
            return NULL;;
        } else {
            if(DEBUG_FLAG) printf("Successfully initialized ADC_LINE(%u)\n", i);
        }
    }
	
	// Sensor 1 INPUT
	 if (gpio_init(GPIO_PIN(0, 13),  GPIO_IN) < 0) {
        if(DEBUG_FLAG) printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 13);
        return NULL;;
    }
	
	// Sensor 2 INPUT
	 if (gpio_init(GPIO_PIN(0, 14),  GPIO_IN) < 0) {
        if(DEBUG_FLAG) printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 14);
        return NULL;;
    }
	
	
	 
    while (1) {
        for (unsigned i = 0; i < ADC_NUMOF; i++) {
            sample = adc_sample(ADC_LINE(i), RES);
            if (sample < 0) {
                if(DEBUG_FLAG) printf("ADC_LINE(%u): 10-bit resolution not applicable\n", i);
            } else {
                if(DEBUG_FLAG) printf("ADC_LINE(%u): %i\n", i, sample);
            }
			
			if(i == 0) {
			if(sample > 50){
				if(DEBUG_FLAG) printf("Someone on the door!! \n");
				xtimer_sleep(1);
				if(DEBUG_FLAG) printf("ADC value is %d \n", sample);
				int sensor1 = gpio_read(GPIO_PIN(0, 13));
				int sensor2 = gpio_read(GPIO_PIN(0, 14));
				
				/*This code is for the raspberry pi*/
            raspberry_pi(sensor1 , sensor2); //this should be sent to rpi as message
			}
			/*This code is for the raspberry pi*/
			}
        }
        xtimer_periodic_wakeup(&last, DELAY);
    }
  
   (void)(argv);
  
return NULL;;
}

void functionMCSensor (void){
	
	sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
	//char *topic = TOPIC_SUP;
	
    puts("MQTT-SN example application\n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");
	
	if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, BROKER_IP) == NULL) {
        if(DEBUG_FLAG) printf("error parsing IPv6 address\n");
        return ;
    }
	
	// LED 1 OUTPUT
    if (gpio_init(GPIO_PIN(0, 23),  GPIO_OUT) < 0) {
        if(DEBUG_FLAG) printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 23);
        return ;
    }
	// LED 2 OUTPUT
	if (gpio_init(GPIO_PIN(0, 28),  GPIO_OUT) < 0) {
        if(DEBUG_FLAG) printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 28);
        return ;
    }
	
	    // Turn off the LED 1 as first state
	    gpio_clear(GPIO_PIN(0,23)); 
        gpio_clear(GPIO_PIN(0,28)); 


	 /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, (sizeof(queue) / sizeof(msg_t)));

    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)) );

    /* start the emcute thread */
    thread_create(publisherStack, sizeof(publisherStack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

				  int notConnected = 1 ;
	
	xtimer_sleep(5);
	while(notConnected){
	if (emcute_con(&gw, true, NULL, NULL, 0, 0) != EMCUTE_OK) {
       if(DEBUG_FLAG) printf("error: unable to connect to Gateway\n");
    }
	else{    notConnected = 0;
	         if(DEBUG_FLAG)
				 printf("Successfully connected to Gateway\n");
			 char *message[] = {"pub","publisher/communication", "\"The SHD microcontroller is live, and ready to communicate.\""};
						 publisherHandler(3, message);
            thread_create(mainStack, sizeof(mainStack),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    adc,
                    NULL, "mainThread");
	}	}	
					return ;
}

int main(void)
{
	functionMCSensor(); // function to simulate sensor mc		
    return 0;
}



