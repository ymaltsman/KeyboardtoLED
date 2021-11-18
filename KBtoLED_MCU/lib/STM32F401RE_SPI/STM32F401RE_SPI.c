// STM32F401RE_SPI.c
// SPI function declarations

#include "STM32F401RE_SPI.h"
#include "STM32F401RE_RCC.h"
#include "STM32F401RE_GPIO.h"

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 

void spiInit(uint32_t br, uint32_t cpol, uint32_t cpha) {
    // Turn on GPIOA and GPIOB clock domains (GPIOAEN and GPIOBEN bits in AHB1ENR)
    RCC->AHB1ENR.GPIOAEN = 1;
    RCC->AHB1ENR.GPIOBEN = 1;
    
    RCC->APB2ENR |= (1 << 12); // Turn on SPI1 clock domain (SPI1EN bit in APB2ENR)

    // Initially assigning SPI pins
    pinMode(GPIOA, 5, GPIO_ALT); // PA5, Arduino D13, SPI1_SCK
    pinMode(GPIOA, 6, GPIO_ALT); // PA6, Arduino D12, SPI1_MISO
    pinMode(GPIOA, 7, GPIO_ALT); // PA7, Arduino D11, SPI1_MOSI
    pinMode(GPIOB, 6, GPIO_OUTPUT); // PB6, Arduino D10, Manual CS

    // Set output speed type to high for SCK
    GPIOA->OSPEEDR |= (0b11 << 2*5);

    // Set to AF05 for SPI alternate functions
    GPIOA->AFRL |= (1 << 18) | (1 << 16);
    GPIOA->AFRL |= (1 << 22) | (1 << 20);
    GPIOA->AFRL |= (1 << 26) | (1 << 24);
    GPIOA->AFRL |= (1 << 30) | (1 << 28);
    

    SPI1->CR1.BR = br;      // Set the clock divisor
    SPI1->CR1.CPOL = cpol;  // Set the polarity
    SPI1->CR1.CPHA = cpha;  // Set the phase
    SPI1->CR1.LSBFIRST = 0; // Set least significant bit first
    SPI1->CR1.DFF = 0;      // Set data format to 8 bits
    SPI1->CR1.SSM = 0;      // Turn off software slave management
    SPI1->CR2.SSOE = 1;     // Set the NSS pin to output mode
    SPI1->CR1.MSTR = 1;     // Put SPI in master mode
    SPI1->CR1.SPE = 1;      // Enable SPI
}

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
uint8_t spiSendReceive(uint8_t send) {
    while(!(SPI1->SR.TXE)); // Wait until the transmit buffer is empty
    SPI1->DR.DR = send; // Transmit the character over SPI
    while(!(SPI1->SR.RXNE)); // Wait until data has been received
    uint8_t rec = SPI1->DR.DR;
    return rec; // Return received character
}

void spiSendColor(uint8_t color){
    while(!(SPI1->SR.TXE)); // Wait until the transmit buffer is empty
    SPI1->DR.DR = color; // Transmit the color over SPI
}

/* Transmits a short (2 bytes) over SPI and returns the received short.
 *    -- send: the short to send over SPI
 *    -- return: the short received over SPI */
uint16_t spiSendReceive16(uint16_t send) {
    digitalWrite(GPIOB, 6, 0);
    SPI1->CR1.SPE = 1;
    SPI1->DR.DR = send;
    
    while(!(SPI1->SR.RXNE));
    uint16_t rec = SPI1->DR.DR;
    
    SPI1->CR1.SPE = 0;
    digitalWrite(GPIOB, 6, 1);

    return rec;
}