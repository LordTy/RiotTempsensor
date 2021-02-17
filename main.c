#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "timex.h"
#include "xtimer.h"
#include "shell.h"

#include "mutex.h"



#include "bmx280.h"
#include "bmx280_params.h"

static bmx280_t bmp280;
static mutex_t bmp_lock = MUTEX_INIT;

static char bmp_stack[THREAD_STACKSIZE_MAIN];

static void *bmp_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */
        mutex_lock(&bmp_lock);

        /* Read the temperature values here */
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        printf("Temperature: %i,%u C\n",(temp/100),(temp%100));

        /* Release the mutex here */
        mutex_unlock(&bmp_lock);

        xtimer_usleep(5 * US_PER_SEC);
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
        printf("Temperature: %i,%u C\n",(temp/100),(temp%100));
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

    thread_create(bmp_stack, sizeof(bmp_stack), THREAD_PRIORITY_MAIN - 1,
                  0, bmp_thread, NULL, "bmp280");

    /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
