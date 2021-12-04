#include "main.h"

int main(void) {
    configureFlash();
    configureClock();

    RCC->AHB1ENR.GPIOAEN = 1;
    pinMode(GPIOA, K_CLK, GPIO_INPUT);
    pinMode(GPIOA, K_DATA, GPIO_INPUT);
    pinMode(GPIOA, SUCCESS_LED, GPIO_OUTPUT);

    digitalWrite(GPIOA, SUCCESS_LED, 1);

    // TODO
    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 14);
    // 2. Set EXTICR2 for PA5
    *SYSCFG_EXTICR2 |= (0x00 << 4);
    //we're working with EXTICR3, which needs to remain at 0

    //enable interrupts globally
    __enable_irq();

    // TODO: Configure interrupt for falling edge of GPIO PA0
    // 1. Configure mask bit
    EXTI->IMR |= (1 << 5);
    // 2. Disable rising edge trigger
    EXTI->RTSR |= (1 << 5);
    // 3. Enable falling edge trigger
    EXTI->FTSR |= (0<<5);
    // 4. Turn on EXTI9-5 interrupt in NVIC_ISER0
    *NVIC_ISER0 |= (1<<23);

    while(1){
        
    }
}

void EXTI9_5_IRQhandler(void){
    digitalWrite(GPIOA, SUCCESS_LED, 0);
    // Check that EXTI_8 was what triggered our interrupt
    if (EXTI->PR & (1 << K_DATA)){
        // If so, clear the interrupt
        EXTI->PR |= (1 << K_DATA);
        
        // Then toggle the LED
        int count = 0;
        ps2_frame_t ps2_frame;
        while(1){
            if (count == 0) ps2_frame.raw = 0;
            while(digitalRead(GPIOA, K_CLK)); //wait for clock signal to go low
            ps2_frame.raw |= digitalRead(GPIOA, K_DATA) << count;
            count++;
            while(!digitalRead(GPIOA, K_CLK));
            if (count == 11){
                if (ps2_frame.data == 0x1C) digitalWrite(GPIOA, SUCCESS_LED, 1);
                count = 0;
            }
            
        }
    }

}

