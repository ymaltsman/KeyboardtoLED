#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"
#include "main.h"

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
void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
double wave_function(int x, int x0, float t);

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

  //keyboard inputs
  pinMode(GPIOB, K_CLK, GPIO_INPUT);
  pinMode(GPIOA, K_DATA, GPIO_INPUT);
  
  
  int count = 0;
  ps2_frame_t ps2_frame;

  //initialize LED array
  uint8_t LED0[N][3];
  uint8_t LED[N][3];
  uint8_t color[3] = {0xff, 0x00, 0x00};
  int x0;
  float t = 500;
  //init_LED(LED0, color);
  //init_LED(LED, color);
  //sendLEDarray(LED);
  while(1){
      while(digitalRead(GPIOB, K_CLK)); //wait for clock signal to go low
      ps2_frame.raw |= digitalRead(GPIOA, K_DATA) << count;
      count++;
      while(!digitalRead(GPIOB, K_CLK));
      if (count == 11){
          if (ps2_frame.data == 0x1C){
              color[0] = 0xFF;
              //color[1] = 0x00;
              //color[2] = 0x00;
              init_LED(LED, color);
              sendLEDarray(LED);
              //x0 = 1;
          } else if (ps2_frame.data == 0x32){
              color[1] = 0xFF;
              //color[0] = 0x00;
              //color[2] = 0x00;
              init_LED(LED, color);
              sendLEDarray(LED);
              //x0 = 2;
          } else if (ps2_frame.data == 0x21){
              color[2] = 0xFF;
              //color[1] = 0x00;
              //color[0] = 0x00;
              init_LED(LED, color);
              sendLEDarray(LED);
              //x0 = 3;
          }
          count = 0;
      }

      
      //process_array(LED, LED0, x0, t);
      //t = t + .01;
  }
}


void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = LED0[i][0] - wave_function(i, x, t)*BASE_COL;
        LED[i][1] = LED0[i][1] - wave_function(i, x, t)*BASE_COL;
        LED[i][2] = LED0[i][2] - wave_function(i, x, t)*BASE_COL;
    }
}

double wave_function(int x, int x0, float t){
    double u;
    double k = (x-t-x0)*(x-t-x0);
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
    //uint8_t send;
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