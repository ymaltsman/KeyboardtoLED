//testing sending data to FPGA for N-pixels LED
//strip. FPGA will then convert 
//data to format for LEDs

#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"

////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////

#define _USE_MATH_DEFINES
#define N 7 //number of pixels in the strip
#define BASE_COL 100 //base color

#define LOAD_PIN    5 //PB

#define MCK_FREQ 100000 //not sure what this is for, may take out

void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);

int main(void){
    // Configure flash latency and set clock to run at 84 MHz
  configureFlash();
  configureClock();

  // Enable GPIOA clock
  RCC->AHB1ENR.GPIOAEN = 1;
  RCC->AHB1ENR.GPIOCEN = 1;

  // "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
  spiInit(1, 0, 0);


  // Load pin
  pinMode(GPIOB, LOAD_PIN, GPIO_OUTPUT);
  
  //initialize LED array
  uint8_t LED[N][3];
  uint8_t color[3] = {100, 100, 100};
  init_LED(LED, color);

  //send LED array to FPGA
  sendLEDarray(LED);


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

    digitalWrite(GPIOB, LOAD_PIN, 1);

    for(i = 0; i<N; i++){
        spiSendColor(LED[i][0]);
        spiSendColor(LED[i][1]);
        spiSendColor(LED[i][2]);
    }
    
    while(SPI1->SR.BSY); // Confirm all SPI transactions are completed
    digitalWrite(GPIOB, LOAD_PIN, 0);
}