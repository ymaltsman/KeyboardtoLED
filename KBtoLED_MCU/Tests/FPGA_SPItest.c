/*
Test receiving data from the FPGA over SPI, sending out color
to FPGA based on input
*/

#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"
#include "main.h"

////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////

#define _USE_MATH_DEFINES

#define N 7 //number of pixels in the strip

#define DONE_PIN 6 //PB6, done signal for MCU to send FPGA when it finishes
#define FPGA_FLAG 8 //PA3, flag from FPGA when it sends interrupt
#define LOAD_PIN 5 //PB5/J13
#define Ready_PIN 10 //PA9, CE for FPGA to send keypress over SPI
//#define TEST_PIN 10

#define MCK_FREQ 100000 //not sure what this is for, may take out

uint8_t getsignal();

int main(void){
    configureFlash();
    configureClock();

    //Enable GPIOA, B, C clocks
    RCC->AHB1ENR.GPIOAEN = 1;
    RCC->AHB1ENR.GPIOBEN = 1;
    RCC->AHB1ENR.GPIOCEN = 1;

    // "clock divide" = master clock frequency / desired baud rate
    // the phase for the SPI clock is 1 and the polarity is 0
    spiInit(1, 0, 0);

    pinMode(GPIOA, Ready_PIN, GPIO_OUTPUT);
    pinMode(GPIOA, FPGA_FLAG, GPIO_INPUT);
    //pinMode(GPIOA, TEST_PIN, GPIO_OUTPUT);
    digitalWrite(GPIOA, Ready_PIN, 0);

    uint8_t data;

    while(!digitalRead(GPIOA, FPGA_FLAG));

    //digitalWrite(GPIOA, Ready_PIN, 1);
    data = getsignal();



}

uint8_t getsignal(){
    uint8_t data;

    digitalWrite(GPIOA, Ready_PIN, 1);
    data = spiSendReceive(0);
    while(SPI1->SR.BSY);
    digitalWrite(GPIOA, Ready_PIN, 0);
    
    return data;
}