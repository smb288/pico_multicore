#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"


const int LED1_PIN = 0, 
          LED2_PIN = 1;

void pinInit(uint firstLED, uint secondLED) {
    gpio_init(firstLED);
    gpio_init(secondLED);
    gpio_set_dir(firstLED, GPIO_OUT);
    gpio_set_dir(secondLED, GPIO_OUT);
}

void secondCoreCode() {
    while(1) {
        gpio_put(LED1_PIN, 1);
        sleep_ms(1000);
        gpio_put(LED1_PIN, 0);
        sleep_ms(1000);
        printf("Second core LED blinked\n");
    }
}

int main() {

    stdio_init_all();
    
    //Initialize pin numbers
    pinInit(LED1_PIN, LED2_PIN);
    
    //Launch second core
    multicore_launch_core1(secondCoreCode);

    //First core code
    while(1) {
        gpio_put(LED2_PIN, 0);
        sleep_ms(500);
        gpio_put(LED2_PIN, 1);
        sleep_ms(500);
        printf("First core LED blinked\n");
    }
}