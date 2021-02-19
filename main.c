#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net/af.h"
#include "net/coap.h"
#include "net/nanocoap.h"
#include "thread.h"
#include "timex.h"
#include "xtimer.h"
#include "shell.h"

#include "mutex.h"

#include "net/gcoap.h"

#include "bmx280.h"
#include "bmx280_params.h"

#ifndef HOSTIP6
    #define HOSTIP6 "2001:db8::1"
#endif

static bmx280_t bmp280;
static mutex_t bmp_lock = MUTEX_INIT;

static char bmp_stack[THREAD_STACKSIZE_MAIN];

coap_pkt_t coap_pkt;
uint8_t coap_buf[64];
sock_udp_ep_t remote = {.family = AF_INET6,
                        .port = 5863,
                        .netif = SOCK_ADDR_ANY_NETIF};


static void *bmp_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */
        mutex_lock(&bmp_lock);
        /* Read the temperature values here */
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        printf("Temperature: %i.%u C\n",(temp/100),(temp%100));


        /* Send temperature as coap message */
        size_t len;
       gcoap_req_init(&coap_pkt, coap_buf, sizeof(coap_buf), COAP_METHOD_POST, "/temp");
       len = coap_opt_finish(&coap_pkt, COAP_OPT_FINISH_PAYLOAD);
       char msg[32]={'\0'};
       sprintf(msg,"%02i.%02u",(temp/100),(temp%100));
       strcpy((char*)coap_pkt.payload,msg);
       len+=strlen(msg);
       gcoap_req_send(coap_buf, len,
                        &remote, NULL, NULL);


        /* Release the mutex here */
        mutex_unlock(&bmp_lock);
        xtimer_sleep(60);
    }

    return 0;
}

static void _bmp280_usage(char *cmd){
    printf("usage: %s <temperature|pressure>\n", cmd);
}

static int bmp280_handler(int argc, char *argv[]){
    (void) argc;
    if (!strcmp(argv[1],"temperature")){
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        printf("Temperature: %i.%u C\n",(temp/100),(temp%100));
    }
    else if (!strcmp(argv[1],"pressure")){
        uint32_t pres = 0;
        pres = bmx280_read_pressure(&bmp280);
        printf("Pressure: %li hPa\n",pres);
    }else{
        _bmp280_usage(argv[0]);
        return -1;
    }
    return 0;
}
static void _bmp280_thread_usage(char *cmd){
    printf("usage: %s <start|stop>\n", cmd);
}

static int bmp280_thread_handler(int argc, char *argv[]){
    (void) argc;
    if (!strcmp(argv[1],"start")){
        mutex_unlock(&bmp_lock);
    }else if (!strcmp(argv[1],"stop")){
        mutex_trylock(&bmp_lock);
    }else{
        _bmp280_thread_usage(argv[0]);
        return -1;
    }
    return 0;
}

static const shell_command_t commands[] = {
    {"bmp","Read the BMP280 values", bmp280_handler},
    {"bmc","Continuesly reads BMP280 temperature", bmp280_thread_handler},
    { NULL, NULL, NULL}
};

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{

    /* Initialize the bmp280 sensor here */
    int err = bmx280_init(&bmp280, bmx280_params);
    if(err==BMX280_OK){
        printf("BMP280 initialized succesfully\n");
    }else if(err==BMX280_ERR_BUS){
        printf("BMP280 BUS ERROR\n");
    }else if(err==BMX280_ERR_NODEV){
        printf("BMP280 device not found\n");
    }

    printf("Coap target is: %s",HOSTIP6);
    ipv6_addr_t addr;
    ipv6_addr_from_str(&addr, HOSTIP6);
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    thread_create(bmp_stack, sizeof(bmp_stack), THREAD_PRIORITY_MAIN - 1,
                  0, bmp_thread, NULL, "bmp280");
    
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}