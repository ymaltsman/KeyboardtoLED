/*
MCU2 captures keypress and sends it to MCU1
*/

#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"
#include "main.h"

#define LOAD_PIN 5 //PB
#define FLAG 3 //PA3
#define DONE_PIN 6

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

  pinMode(GPIOA, K_CLK, GPIO_INPUT);
  pinMode(GPIOA, K_DATA, GPIO_INPUT);
  pinMode(GPIOB, DONE_PIN, GPIO_INPUT);
  pinMode(GPIOA, FLAG, GPIO_OUTPUT);
  pinMode(GPIOA, SUCCESS_LED, GPIO_OUTPUT);
  
  digitalWrite(GPIOA, SUCCESS_LED, 0);
  digitalWrite(GPIOA, FLAG, 0);
  // Load pin
  pinMode(GPIOB, LOAD_PIN, GPIO_OUTPUT);
  

  int count = 0;
  ps2_frame_t ps2_frame;
  uint8_t press;
  while(1){
      //poll keyboard
      if (count == 0) ps2_frame.raw = 0;
      while(digitalRead(GPIOA, K_CLK)); //wait for clock signal to go low
      ps2_frame.raw |= digitalRead(GPIOA, K_DATA) << count;
      count++;
      while(!digitalRead(GPIOA, K_CLK));
      if (count == 11){
          press = ps2_frame.data;
          if (ps2_frame.data == 0x1C) digitalWrite(GPIOA, SUCCESS_LED, 1);
          count = 0;
          digitalWrite(GPIOA, FLAG, 1);
          while(!digitalRead(GPIOB, DONE_PIN)); //wait for MCU to be ready
          uint8_t send;
          send = spiSendReceive(press);
      }







  }
}