#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

const int LED1_PIN = 0, 
          LED2_PIN = 1,
          TRIG_PIN = 2,
          ECHO_PIN = 3;

bool TOO_CLOSE = false;
int timeout = 26100;

void pinInit(uint firstLED, uint secondLED, uint trigPin, uint echoPin) {
    gpio_init(firstLED);
    gpio_init(secondLED);
    gpio_init(trigPin);
    gpio_init(echoPin);

    gpio_set_dir(firstLED, GPIO_OUT);
    gpio_set_dir(secondLED, GPIO_OUT);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

void secondCoreCode() {
    while(1) {
        gpio_put(LED1_PIN, 1);
        sleep_ms(100);
        gpio_put(LED1_PIN, 0);
        sleep_ms(100);
        if(TOO_CLOSE == true) gpio_put(LED2_PIN, 1);
        else gpio_put(LED2_PIN, 0);
        printf("Second core done\n");
    }
}

int main() {

    stdio_init_all();

    //Initialize pin numbers
    pinInit(LED1_PIN, LED2_PIN, TRIG_PIN, ECHO_PIN);
    
    //Launch second core
    multicore_launch_core1(secondCoreCode);

    //First core code
    while(1) {
        gpio_put(TRIG_PIN, 1);
        sleep_us(10);
        gpio_put(TRIG_PIN, 0);

        uint width = 0;

        while (gpio_get(ECHO_PIN) == 0) tight_loop_contents();
        while (gpio_get(ECHO_PIN) == 1) {
            width++;
            sleep_us(1);
            if (width > timeout) return 0;
        }
        int cmLength = width / 29 / 2;
        printf("%d cm\n", cmLength);
        if(cmLength < 20) TOO_CLOSE = true;
        else TOO_CLOSE = false;
        printf("First core done.\n");
        sleep_ms(100);
    }
}