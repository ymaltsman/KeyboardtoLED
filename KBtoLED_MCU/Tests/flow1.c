/*
Test full flow for polling FPGA and sending array.
Not scalable to multiple keypresses that follow quickly after one another.
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
#define BASE_COL 255
#define K 2 //number of keys


#define READY_PIN 9 //PA9, CE for FPGA to send keypress over SPI
#define DONE_PIN 6 //PB6, done signal for MCU to send FPGA when it finishes
#define FPGA_FLAG 10 //PB10, flag from FPGA when it sends interrupt
#define LOAD_PIN 5 //PB5, CE for sending color to FPGA

#define MCK_FREQ 100000 //not sure what this is for, may take out

void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);
void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
double wave_function(int x, int x0, float t);
int keytoloc(key keys[K], uint8_t data);
uint8_t getkey();
void init_keys(key keys[K]);

void init_keys(key keys[K]){
    keys[0].data = 0x1C;
    keys[0].loc = 0;

    keys[1].data = 0x32;
    keys[1].loc = 1;
}



int main(void){
      // Configure flash latency and set clock to run at 84 MHz
  configureFlash();
  configureClock();
  
  key keys[K];
  init_keys(keys);
  // Enable GPIOA clock
  RCC->AHB1ENR.GPIOAEN = 1;
  RCC->AHB1ENR.GPIOBEN = 1;
  RCC->AHB1ENR.GPIOCEN = 1;

  // "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
  spiInit(1, 0, 0);
  
  
  pinMode(GPIOB, DONE_PIN, GPIO_OUTPUT); //configure done pin
  digitalWrite(GPIOB, DONE_PIN, 0);
  
  pinMode(GPIOB, FPGA_FLAG, GPIO_INPUT); //configure flag
  pinMode(GPIOA, READY_PIN, GPIO_OUTPUT);
  pinMode(GPIOB, LOAD_PIN, GPIO_OUTPUT);
  
  //initialize LED array
  uint8_t LED0[N][3];
  uint8_t LED[N][3];
  uint8_t color[3] = {0xFF, 0xFF, 0xFF};

  float t = 0;
  int x0;
  float dt = 0.5;
  float end = 2;

  init_LED(LED0, color);
  init_LED(LED, color);

  uint8_t data;

  int test_mode = 1; //for debugging without the FPGA

  while(1){
      if (digitalRead(GPIOB, FPGA_FLAG)){
          
          data = getkey();
          x0 = keytoloc(keys, data);
          t = .01;
      } else if (test_mode == 1){ //for debugging without the FPGA
          data = 0x1C;
          x0 = keytoloc(keys, data);
          t = .01;
          test_mode = 0;
      }
      if (t == 0){
          sendLEDarray(LED);
      }
      else if (t >= end){
          t = 0;
      } else {
          process_array(LED, LED0, x0, t);
          sendLEDarray(LED);
          t += dt;
      }
  }


}

uint8_t getkey(){
    uint8_t data;
    digitalWrite(GPIOA, READY_PIN, 1);
    data = spiSendReceive(0);
    while(SPI1->SR.BSY);
    digitalWrite(GPIOA, READY_PIN, 0);
          
    digitalWrite(GPIOB, DONE_PIN, 1);
    while (digitalRead(GPIOB, FPGA_FLAG)); //wait for FPGA to lower interrupt flag
    digitalWrite(GPIOB, DONE_PIN, 0);
    return data;
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
    double w = 1;
    double k = (x-t-x0)*(x-t-x0);
    double u = exp(-k/(2*w*w));
    return u;
}

int keytoloc(key keys[K], uint8_t data){
    /*starting with a list implementation for simplicity, may switch
    to dictionary for efficiency.
    Need to have all keys covered for this to work, otherwise we'll return x0 unitialized 
    */
   int j;
   int x0;
   for (j = 0; j< K; j++){
       if (keys[j].data == data){
           x0 = keys[j].loc;
       }
   }
   return x0;
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