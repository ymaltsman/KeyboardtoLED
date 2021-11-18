// STM32F401RE_USART.c
// Source code for USART functions

#include "STM32F401RE_USART.h"
#include "STM32F401RE_GPIO.h"
#include "STM32F401RE_RCC.h"

/*
    id2Port takes a USART_ID (index of USART peripheral) and returns a pointer
    to a USART-sized chunk of memory at the correct base address for that
    peripheral.
*/
USART_TypeDef * id2Port(int USART_ID){
    USART_TypeDef * USART;
    switch(USART_ID){
        case(USART1_ID) :
            USART = USART1;
            break;
        case(USART2_ID) :
            USART = USART2;
            break;
        default:
            USART = 0;
    }
    return USART;
}

/*
    initUSART sets up the USART peripheral and configures the pins, sets the 
    appropriate configuration values, and enables the peripheral.
*/
USART_TypeDef * initUSART(int USART_ID){
    RCC->AHB1ENR.GPIOAEN = 1; // Enable GPIO port A

    USART_TypeDef * USART = id2Port(USART_ID);

    switch(USART_ID){
        case USART1_ID :
            RCC->APB2ENR |= (1 << 4); // Set USART1EN

            // Configure pin modes as ALT function
            pinMode(GPIOA, 9, GPIO_ALT);  // TX D8
            pinMode(GPIOA, 10, GPIO_ALT); // RX D2

            GPIOA->AFRH |= (0b111 << 4*2) | (0b111 << 4*1);
            break;
        case USART2_ID :
            RCC->APB1ENR |= (1 << 17); // Set USART2EN

            // Configure pin modes as ALT function
            pinMode(GPIOA, 2, GPIO_ALT); // D1 TX
            pinMode(GPIOA, 3, GPIO_ALT); // D0 RX

            // Configure correct alternate functions (AF07)
            GPIOA->AFRL |= (0b111 << 4*3) | (0b111 << 4*2);
            break;
    }

    USART->CR1.UE = 1; // Enable USART
    USART->CR1.M = 0; // M=0 corresponds to 8 data bits
    USART->CR2.STOP = 0b00; // 0b00 corresponds to 1 stop bit
    USART->CR1.OVER8 = 0; // Set to 16 times sampling freq

    // Set baud rate to 9600 bps
    // Tx/Rx baud = (f_CK)/(8*(2-OVER8)*USARTDIV) = Tx/Rx baud = (f_CK)/(16*USARTDIV)
    if(USART_ID == USART1_ID){
      // f_CK = 84e6 Hz on APB2 (USART1)
      // USARTDIV = 84e6/(16*BAUD_RATE) = 546.875
      // DIV_Mantissa = 546 = 0x222
      // DIV_Fraction = 0.875 = 14/16 = 0b1110
        USART->BRR.DIV_Mantissa = 546;
        USART->BRR.DIV_Fraction = 14;
    }
    else if (USART_ID == USART2_ID){
      // f_CK = 42e6 on APB1 (USART2)
      // USARTDIV =42e6/(16*BAUD_RATE) = 273.4375
      // DIV_Mantissa = 273 = 0x111
      // DIV_Fraction = 0.4375 = 7/16 = 0b0111
        USART->BRR.DIV_Mantissa = 273;
        USART->BRR.DIV_Fraction = 7;
    }

    USART->CR1.TE = 1; // Enable transmission
    USART->CR1.RE = 1; // Enable reception

    return USART;
}

/*
    sendChar takes a pointer to a USART peripheral and an 8-bit data packet
    and outputs the data packet over the serial interface.
    It then waits for the transmission to be complete to exit the function.
*/
void sendChar(USART_TypeDef * USART, uint8_t data){
    while(!USART->SR.TXE);
    USART->DR.DR = data;
    while(!USART->SR.TC);
}

/*
    receiveChar takes a pointer to a USART peripheral and
    and if there is data in the receive buffer, reads and returns the character.
    Otherwise, it returns 0.
*/
uint8_t receiveChar(USART_TypeDef * USART) {
  if(USART->SR.RXNE) return (uint8_t) USART->DR.DR;
  else               return (uint8_t) 0;
}