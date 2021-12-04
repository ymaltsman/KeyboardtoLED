#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"

////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////

#define _USE_MATH_DEFINES
#define N 7 //number of pixels in the strip
#define BASE_COL 255 //base color

#define LOAD_PIN    5 //PB

#define MCK_FREQ 100000 //not sure what this is for, may take out

void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);
void process_array(uint8_t LED[N][3], int x, float t);
double wave_function(int x, int x0, float t);

int main(void){
    configureFlash();
    configureClock();

    // Enable GPIOA clock
    RCC->AHB1ENR.GPIOAEN = 1;
    RCC->AHB1ENR.GPIOCEN = 1;

    // "clock divide" = master clock frequency / desired baud rate
    // the phase for the SPI clock is 1 and the polarity is 0
    spiInit(1, 0, 0);

    //keyboard inputs, success LED
    pinMode(GPIOA, K_CLK, GPIO_INPUT);
    pinMode(GPIOA, K_DATA, GPIO_INPUT);
    pinMode(GPIOA, SUCCESS_LED, GPIO_OUTPUT);

    digitalWrite(GPIOA, SUCCESS_LED, 0);

    // Load pin
    pinMode(GPIOB, LOAD_PIN, GPIO_OUTPUT);
    
    //initialize LED array
    uint8_t LED[N][3];
    uint8_t color[3] = {0xFF, 0xFF, 0xFF};

    while(1){

    }

}