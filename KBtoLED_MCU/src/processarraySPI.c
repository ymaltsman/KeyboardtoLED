#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"

////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////

#define _USE_MATH_DEFINES
#define N 3 //number of pixels in the strip
#define BASE_COL 100 //base color

#define LOAD_PIN    5 //PB

#define MCK_FREQ 100000 //not sure what this is for, may take out

void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);
void process_array(uint8_t LED[N][3], int x, int t);
double wave_function(int x, int x0, int t);

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
  uint8_t color[3] = {0x55, 0x55, 0x55};
  int x = 2;
  int t = 0;
  init_LED(LED, color);
  while(t < 3){
      sendLEDarray(LED);
      process_array(LED, x, t);
      t++;
  }
}


void process_array(uint8_t LED[N][3], int x, int t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = wave_function(i, x, t)*BASE_COL;
        LED[i][1] = wave_function(i, x, t)*BASE_COL;
        LED[i][2] = wave_function(i, x, t)*BASE_COL;
    }
}

double wave_function(int x, int x0, int t){
    double u;
    double k = (x-x0)*(x-x0);
    u = (1/sqrt(4*M_PI*t))*exp(-k/(4*t));
    return u;
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