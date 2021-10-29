#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

const int MOTOR1_FW = 0, 
          MOTOR1_BW = 1,
          TRIG_PIN = 2,
          ECHO_PIN = 3,
          MOTOR2_FW = 4,
          MOTOR2_BW = 5;

bool TOO_CLOSE = false;
int timeout = 26100;

void pinInit(uint firstMotorF, uint firstMotorB, uint trigPin, uint echoPin,
    uint secondMotorF, uint secondMotorB) {

    gpio_init(firstMotorF);
    gpio_init(firstMotorB);
    gpio_init(secondMotorF);
    gpio_init(secondMotorB);

    gpio_init(trigPin);
    gpio_init(echoPin);

    gpio_set_dir(firstMotorF, GPIO_OUT);
    gpio_set_dir(firstMotorB, GPIO_OUT);
    gpio_set_dir(secondMotorF, GPIO_OUT);
    gpio_set_dir(secondMotorB, GPIO_OUT);

    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

void forwards() {
    gpio_put(MOTOR1_FW, 1);
    gpio_put(MOTOR1_BW, 0);
    gpio_put(MOTOR2_FW, 1);
    gpio_put(MOTOR2_BW, 0);
}

void backwards() {
    gpio_put(MOTOR1_FW, 0);
    gpio_put(MOTOR1_BW, 1);
    gpio_put(MOTOR2_FW, 0);
    gpio_put(MOTOR2_BW, 1);
}

void stop_motors() {
    gpio_put(MOTOR1_FW, 0);
    gpio_put(MOTOR1_BW, 0);
    gpio_put(MOTOR2_FW, 0);
    gpio_put(MOTOR2_BW, 0);
}

void turn_right() {
    gpio_put(MOTOR1_FW, 1);
    gpio_put(MOTOR1_BW, 0);
    gpio_put(MOTOR2_FW, 0);
    gpio_put(MOTOR2_BW, 0);
}

void turn_left() {
    gpio_put(MOTOR1_FW, 0);
    gpio_put(MOTOR1_BW, 0);
    gpio_put(MOTOR2_FW, 1);
    gpio_put(MOTOR2_BW, 0);
}

void secondCoreCode() {
    while(1) {
        if(TOO_CLOSE) stop_motors();
        else forwards();
        sleep_us(10);
    }
}

int main() {

    stdio_init_all();

    //Initialize pin numbers
    pinInit(MOTOR1_FW, MOTOR1_BW, TRIG_PIN, ECHO_PIN, 
            MOTOR2_FW, MOTOR2_BW);
    
    //Launch second core and set clock to 250MHz
    set_sys_clock_khz(250000, true);
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
        }
        int cmLength = width / 29 / 2;

        if(cmLength < 20) TOO_CLOSE = true;
        else TOO_CLOSE = false;
        sleep_ms(10);
    }
    return 0;
}