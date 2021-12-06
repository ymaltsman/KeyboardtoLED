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
#define FPGA_FLAG 3 //PA3, flag from FPGA when it sends interrupt
#define LOAD_PIN 5 //PB
#define Ready_PIN 9 //PA9, CE for FPGA to send keypress over SPI


#define MCK_FREQ 100000 //not sure what this is for, may take out

void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);


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

    pinMode(GPIOB, DONE_PIN, GPIO_OUTPUT); //configure done pin
    digitalWrite(GPIOB, DONE_PIN, 0);

    pinMode(GPIOA, FPGA_FLAG, GPIO_INPUT); //configure flag
    pinMode(GPIOA, Ready_PIN, GPIO_OUTPUT); 

    //initialize LED array 
    uint8_t LED[N][3];
    uint8_t color[3] = {0, 0, 0};
    
    uint8_t data;
    while(1){
        if (digitalRead(GPIOA, FPGA_FLAG)){
            digitalWrite(GPIOA, Ready_PIN, 1);
            data = spiSendReceive(0);
            while(SPI1->SR.BSY);
            digitalWrite(GPIOA, Ready_PIN, 0);

            digitalWrite(GPIOB, DONE_PIN, 1); //can also have FPGA lower it's interrupt flag on it's own
            while (digitalRead(GPIOA, FPGA_FLAG)); //wait for FPGA to lower interrupt flag
            digitalWrite(GPIOB, DONE_PIN, 0);

            if (data == 0x1C){ 
                color[0] = 0xFF;
            } else if (data == 0x32){
                color[1] = 0xFF;
            }
            else{
                color[2] = 0xFF;
            }
        }
        init_LED(LED, color);
        sendLEDarray(LED);

        
    }

}

void init_LED(uint8_t LED[N][3], uint8_t color[3]){ 
    //initialize LED array to be the same color
    int i;

    for (i = 0; i < N; i++){
        LED[i][0] = color[0];
        LED[i][1] = color[1];
        LED[i][2] = color[2];
    }

}

void sendLEDarray(uint8_t LED[N][3]){
    int i;

    int j;
    digitalWrite(GPIOB, LOAD_PIN, 1);
    for(i = 0; i<N; i++){
        for(j = 0; j<3; j++){
            
            spiSendColor(LED[i][j]);
            while(SPI1->SR.BSY); // Confirm all SPI transactions are completed
            
        }
    }
    digitalWrite(GPIOB, LOAD_PIN, 0);
    
}