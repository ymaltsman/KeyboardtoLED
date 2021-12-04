#include <stdio.h>
#include "main.h"


int main(void){
    configureFlash();
    configureClock();

    RCC->AHB1ENR.GPIOAEN = 1;
    pinMode(GPIOA, K_CLK, GPIO_INPUT);
    pinMode(GPIOA, K_DATA, GPIO_INPUT);
    pinMode(GPIOA, SUCCESS_LED, GPIO_OUTPUT);

    digitalWrite(GPIOA, SUCCESS_LED, 0);
    int count = 0;
    ps2_frame_t ps2_frame;
    while(1){
        if (count == 0) ps2_frame.raw = 0;
        while(digitalRead(GPIOA, K_CLK)); //wait for clock signal to go low
        ps2_frame.raw |= digitalRead(GPIOA, K_DATA) << count;
        count++;
        while(!digitalRead(GPIOA, K_CLK));
        if (count == 11){
            if (ps2_frame.data == 0x1C) digitalWrite(GPIOA, SUCCESS_LED, 1);
            count = 0;
        }
        
    }

}