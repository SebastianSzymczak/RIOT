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

#define TEST_OUTPUT_STDOUT 1
#define TEST_OUTPUT_SDL 2
#define TEST_OUTPUT_SPI 3
#define TEST_OUTPUT_I2C 4

#ifndef TEST_OUTPUT
#error "TEST_OUTPUT not defined"
#endif

#if TEST_OUTPUT == TEST_OUTPUT_I2C
#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif
#ifndef TEST_ADDR
#error "TEST_ADDR not defined"
#endif
#ifndef TEST_DISPLAY
#error "TEST_DISPLAY not defined"
#endif
#ifndef TEST_PIN_RESET
#error "TEST_PIN_RESET not defined"
#endif
#endif

#if TEST_OUTPUT == TEST_OUTPUT_SPI
#ifndef TEST_SPI
#error "TEST_SPI not defined"
#endif
#ifndef TEST_DISPLAY
#error "TEST_DISPLAY not defined"
#endif
#ifndef TEST_PIN_CS
#error "TEST_PIN_CS not defined"
#endif
#ifndef TEST_PIN_DC
#error "TEST_PIN_DC not defined"
#endif
#ifndef TEST_PIN_RESET
#error "TEST_PIN_RESET not defined"
#endif
#endif

#include <stdio.h>

#include "periph/gpio.h"
#if TEST_OUTPUT == TEST_OUTPUT_SPI
#include "periph/spi.h"
#endif
#if TEST_OUTPUT == TEST_OUTPUT_I2C
#include "periph/i2c.h"
#endif

#include "xtimer.h"
#include "u8g2.h"

#include <stdio.h>
#include <string.h>
#include "benchmark.h"
#include "irq.h"
#include "shell.h"
#include "led.h"
#include "xtimer.h"
#include "periph/gpio.h"
#include <stdlib.h>
#include "timex.h"
//#include "periph/adc.h"
#include "thread.h"

//part of emcute
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

#define BROKER_IP          ("fdaa:bb:cc:ee::3")
//#define BROKER_PORT        ("1883")
// END of EMCUTE

//#define RES             	ADC_RES_10BIT
#define DELAY 				(100LU * US_PER_MS) 
//#define BROKER_IP			"fdaa:bb:cc:ee::3"
#define TOPIC_SUB_LIGHT_1	"subscriber/light_1"
#define TOPIC_SUB_LIGHT_2	"subscriber/light_2" 

//emcute static data
//static char stack4[THREAD_STACKSIZE_DEFAULT];
//static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];
//static char topics[NUMOFSUBS][TOPIC_MAXLEN];
//

static char stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];

char stack1[THREAD_STACKSIZE_MAIN];
char stack2[THREAD_STACKSIZE_MAIN];
char stack3[THREAD_STACKSIZE_MAIN];
char roomNumber = '0';
short roomOneTime = 0;
short roomTwoTime = 0;
int roomsTimeArray[2];

static const uint8_t logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x3C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF0, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0xF8,
    0x30, 0x3C, 0x3F, 0xC0, 0x00, 0x0C, 0x77, 0xF0, 0x38, 0x7E, 0x3F, 0xC0,
    0x00, 0x7E, 0x73, 0xC0, 0x38, 0xE7, 0x06, 0x00, 0x00, 0xFC, 0x71, 0x00,
    0x38, 0xE3, 0x06, 0x00, 0x01, 0xF0, 0x70, 0x00, 0x38, 0xE3, 0x06, 0x00,
    0x01, 0xC0, 0x70, 0x00, 0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0xC0,
    0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x71, 0xE0, 0x38, 0xE3, 0x06, 0x00,
    0x03, 0x80, 0x70, 0xE0, 0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0xF0,
    0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0x70, 0x38, 0xE3, 0x06, 0x00,
    0x03, 0x80, 0xF0, 0x78, 0x38, 0xE3, 0x06, 0x00, 0x03, 0xC1, 0xE0, 0x3C,
    0x38, 0xE7, 0x06, 0x00, 0x01, 0xE3, 0xE0, 0x3C, 0x38, 0x7E, 0x06, 0x00,
    0x01, 0xFF, 0xC0, 0x1C, 0x30, 0x3C, 0x06, 0x00, 0x00, 0x7F, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

#if (TEST_OUTPUT == TEST_OUTPUT_I2C) || (TEST_OUTPUT == TEST_OUTPUT_SPI)
/**
 * @brief   RIOT-OS pin maping of U8g2 pin numbers to RIOT-OS GPIO pins.
 * @note    To minimize the overhead, you can implement an alternative for
 *          u8x8_gpio_and_delay_riotos.
 */
static gpio_t pins[] = {
    [U8X8_PIN_CS] = TEST_PIN_CS,
    [U8X8_PIN_DC] = TEST_PIN_DC,
    [U8X8_PIN_RESET] = TEST_PIN_RESET
};

/**
 * @brief   Bit mapping to indicate which pins are set.
 */
static uint32_t pins_enabled = (
    (1 << U8X8_PIN_CS) +
    (1 << U8X8_PIN_DC) +
    (1 << U8X8_PIN_RESET)
);
#endif


static const char MICRO_CONTROLLER_1[] = "MICRO_CONTROLLER_1";

// emcute part
static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


/*static unsigned get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}*/

/*static int cmd_con(int argc, char **argv)
{
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 2) {
        printf("usage: %s <ipv6 addr> [port] [<will topic> <will message>]\n", argv[0]);
        return 1;
    }

     parse address 
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, argv[1]) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (argc >= 3) {
        gw.port = atoi(argv[2]);
    }
    if (argc >= 5) {
        topic = argv[3];
        message = argv[4];
        len = strlen(message);
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", argv[1], (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n",
           argv[1], (int)gw.port);
    return 0;
}*/


/*static int cmd_pub(int argc, char **argv)
{
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    if (argc < 3) {
        printf("usage: %s <topic name> <data> [QoS level]\n", argv[0]);
        return 1;
    }

     parse QoS level 
    if (argc >= 4) {
        flags |= get_qos(argv[3]);
    }

    printf("pub with topic: %s and name %s and flags 0x%02x\n", argv[1], argv[2], (int)flags);

     step 1: get topic id 
    t.name = argv[1];
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        return 1;
    }

     step 2: publish data 
    if (emcute_pub(&t, argv[2], strlen(argv[2]), flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [%i]'\n",
            (int)strlen(argv[2]), t.name, t.id);

    return 0;
}*/


char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/*static int raspberry_pi(int sensor1 , int sensor2){
	


	            // send the message to the controller mc as raspberry_pi(int sensor1 , int sensor2)
                if(sensor1 && sensor2){
					  gpio_set(GPIO_PIN(0,23));
					  gpio_set(GPIO_PIN(0,28));
					  char *message[] = {"pub","publisher/bothOn","1"};
						 cmd_pub(3, message);
				  }
				// Reading from the infrared sensor  1
				   else if (sensor1) {
		                 printf("Movement has been detected in room 1\n");
						 roomNumber = '1';
				         gpio_set(GPIO_PIN(0,23));
						 gpio_clear(GPIO_PIN(0,28));
						 	char *message[] = {"pub","publisher/roomSensorInformation1","1"};
						 cmd_pub(3, message);
                           }
				// Reading from the infrared sensor  2  
		            else if (sensor2) {
		                 printf("Movement has been detected in room 2\n");
						 roomNumber = '2';
						 gpio_set(GPIO_PIN(0,28));
				         gpio_clear(GPIO_PIN(0,23));
						 char *message[] = {"pub","publisher/roomSensorInformation2","1"};
						 cmd_pub(3, message);
                           }
						    xtimer_sleep(2);	
						   return 0;
};

void *adc(void *argv){

    printf("%s is ON \n" , MICRO_CONTROLLER_1);
    xtimer_ticks32_t last = xtimer_now();
    int sample = 0;

    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test will sample all available ADC lines once every 100ms with\n"
         "a 10-bit resolution and print the sampled results to STDIO\n\n");
 
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            printf("Initialization of ADC_LINE(%u) failed\n", i);
            return NULL;;
        } else {
            printf("Successfully initialized ADC_LINE(%u)\n", i);
        }
    }
	
	// Sensor 1 INPUT
	 if (gpio_init(GPIO_PIN(0, 13),  GPIO_IN) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 13);
        return NULL;;
    }
	
	// Sensor 2 INPUT
	 if (gpio_init(GPIO_PIN(0, 14),  GPIO_IN) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 14);
        return NULL;;
    }
	
	
	 
    while (1) {
        for (unsigned i = 0; i < ADC_NUMOF; i++) {
            sample = adc_sample(ADC_LINE(i), RES);
            if (sample < 0) {
                printf("ADC_LINE(%u): 10-bit resolution not applicable\n", i);
            } else {
                printf("ADC_LINE(%u): %i\n", i, sample);
            }
			
			if(i == 0) {
			if(sample > 50){
				printf("Someone on the door!! \n");
				xtimer_sleep(1);
				printf("ADC value is %d \n", sample);
				int sensor1 = gpio_read(GPIO_PIN(0, 13));
				int sensor2 = gpio_read(GPIO_PIN(0, 14));
				
				This code is for the raspberry pi
            raspberry_pi(sensor1 , sensor2); //this should be sent to rpi as message
			}
			This code is for the raspberry pi
			}
        }
        xtimer_periodic_wakeup(&last, DELAY);
    }
  
   (void)(argv);
  
return NULL;;
}*/

void *timeLEDIsOn(void *arg)
{
	(void) arg;
	  while(1){
		  if(roomNumber == '1'){
			  xtimer_sleep(1);
			  roomsTimeArray[0]++;
		  }
			  else if(roomNumber == '2'){
				  xtimer_sleep(1);
				  roomsTimeArray[1]++;
				  
			  }
			  
			  else{
				  xtimer_sleep(1);	  
			  }
	     
		  printf("Room 1 was on for:  %d \n", roomsTimeArray[0]);
		  printf("Room 2 was on for:  %d \n", roomsTimeArray[1]);
	  }
	    return 0;
}



short powerConsumtion(void){

	if(roomNumber == '1'){
		return (roomsTimeArray[0] * 100/ (roomsTimeArray[0] + roomsTimeArray[1]));
	}
	else if(roomNumber == '2'){
		return (roomsTimeArray[1] * 100/ (roomsTimeArray[0] + roomsTimeArray[1]));
	}
	return 0; 
}

char * append(char * string1, char * string2)
{
    char * result = NULL;
    asprintf(&result, "%s%s", string1, string2);
    return result;
}

void *monitorThread(void *arg)
{
	(void) arg;
    uint32_t screen = 0;
    u8g2_t u8g2;

	    
					
    /* initialize to stdout */
#if TEST_OUTPUT == TEST_OUTPUT_STDOUT
    puts("Initializing to stdout.");

    u8g2_SetupBuffer_Utf8(&u8g2, U8G2_R0);
#endif

    /* initialize to virtual SDL (native only) */
#if TEST_OUTPUT == TEST_OUTPUT_SDL
    puts("Initializing to SDL.");

    u8g2_SetupBuffer_SDL_128x64_4(&u8g2, U8G2_R0);
#endif

    /* initialize to SPI */
#if TEST_OUTPUT == TEST_OUTPUT_SPI
    puts("Initializing to SPI.");

    TEST_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);

    u8g2_SetPins(&u8g2, pins, pins_enabled);
    u8g2_SetDevice(&u8g2, SPI_DEV(TEST_SPI));
#endif

    /* initialize to I2C */
#if TEST_OUTPUT == TEST_OUTPUT_I2C
    puts("Initializing to I2C.");

    TEST_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_i2c, u8x8_gpio_and_delay_riotos);

    u8g2_SetPins(&u8g2, pins, pins_enabled);
    u8g2_SetDevice(&u8g2, I2C_DEV(TEST_I2C));
    u8g2_SetI2CAddress(&u8g2, TEST_ADDR);
#endif

    /* initialize the display */
    puts("Initializing display.");

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    /* start drawing in a loop */
    puts("Drawing on screen.");
    char message[] = "Room:  ";
    while (1) {
        u8g2_FirstPage(&u8g2);

        do {
            u8g2_SetDrawColor(&u8g2, 1);
            u8g2_SetFont(&u8g2, u8g2_font_helvB12_tf);

            switch (screen) {
                case 0:
                    u8g2_DrawStr(&u8g2, 5, 22, "Someone");
					u8g2_DrawStr(&u8g2, 5, 40, "in:");
                    
                    break;
                case 1:
                    message[6] = roomNumber;
                    u8g2_DrawStr(&u8g2, 5, 22, message);
					char str[255];
					short powerConsumtionVar = powerConsumtion();
                    sprintf(str, "%d", powerConsumtionVar);
				    u8g2_DrawStr(&u8g2, 5, 40, append(str, "%"));
					break;
                case 2:
                    u8g2_DrawBitmap(&u8g2, 0, 0, 8, 32, logo);
                    break;
            }
        } while (u8g2_NextPage(&u8g2));

#if TEST_OUTPUT == TEST_OUTPUT_STDOUT
        /* transfer screen buffer to stdout */
        utf8_show();
#endif

#if TEST_OUTPUT == TEST_OUTPUT_SDL
        /* handle SDL events */
        u8g_sdl_get_key();
#endif
        /* show screen in next iteration */
        screen = (screen + 1) % 3;
        /* sleep a little */
        xtimer_sleep(3);
    }
    return 0;
}


/*void functionMCSensor (void){
	  the main thread needs a msg queue to be able to run `ping6`
    msg_init_queue(queue, (sizeof(queue) / sizeof(msg_t)));

     initialize our subscription buffers 
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)) );

     start the emcute thread 
    thread_create(stack4, sizeof(stack4), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

		    char *args[] = {"con",BROCKER_IP,BROCKER_PORT,"",""};				
					 if(!cmd_con(5, args)){
						 puts("connection has been established");
					char *message[] = {"pub","publisher/communication", "\"The SHD microcontroller is live, and ready to communicate.\""};
						 cmd_pub(3, message);
					 }
					 else{ puts("error in connection!!!"); }
	return ;
}*/


/*void functionMCController (void){
		
	// LED 1 OUTPUT
    if (gpio_init(GPIO_PIN(0, 23),  GPIO_OUT) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 23);
        return ;
    }
	// LED 2 OUTPUT
	if (gpio_init(GPIO_PIN(0, 28),  GPIO_OUT) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", 0, 28);
        return ;
    }
	    // Turn off the LED 1 as first state
	    gpio_clear(GPIO_PIN(0,23)); 
	    gpio_clear(GPIO_PIN(0,28)); 
	
		thread_create(stack1, sizeof(stack1),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    monitorThread,
                    NULL, "monitorThread");
					
		thread_create(stack3, sizeof(stack3),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    timeLEDIsOn,
                    NULL, "timeLEDIsOn");
	return ;
}*/


/*int main(void)
{
	functionMCSensor(); // function to simulate sensor mc
	functionMCController(); // function to simulate the controller mc
		
    return 0;
}*/


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
	
	thread_create(stack1, sizeof(stack1),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    monitorThread,
                    NULL, "monitorThread");
					
		thread_create(stack3, sizeof(stack3),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    timeLEDIsOn,
                    NULL, "timeLEDIsOn");

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



