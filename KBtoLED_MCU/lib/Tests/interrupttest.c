#include "main.h"

int main(void) {
    configureFlash();
    configureClock();

    RCC->AHB1ENR.GPIOAEN = 1;
    pinMode(GPIOA, K_CLK, GPIO_INPUT);
    pinMode(GPIOA, K_DATA, GPIO_INPUT);
    pinMode(GPIOA, SUCCESS_LED, GPIO_OUTPUT);

    // TODO
    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 14);
    // 2. Set EXTICR2 for PA5
    *SYSCFG_EXTICR2 |= (0x00 << 4);
    //we're working with EXTICR3, which needs to remain at 0

    //enable interrupts globally
    __enable_irq();

    // TODO: Configure interrupt for falling edge of GPIO PA8
    // 1. Configure mask bit
    EXTI->IMR |= (1 << 5);
    // 2. Disable rising edge trigger
    EXTI->RTSR |= (0 << 5);
    // 3. Enable falling edge trigger
    EXTI->FTSR |= (1<<5);
    // 4. Turn on EXTI interrupt in NVIC_ISER1
    *NVIC_ISER1 = 1;

    while(1){
        
    }
}

void EXTI3_IRQhandler(void){
    digitalWrite(GPIOA, SUCCESS_LED, 1);
    // Check that EXTI_8 was what triggered our interrupt
    if (EXTI->PR & (1 << K_DATA)){
        // If so, clear the interrupt
        EXTI->PR |= (1 << K_DATA);
        
        // Then toggle the LED
        uint8_t press = 0x00;
        int i = 0;
        int phase = 0;
        int clk_state = digitalRead(GPIOA, K_CLK);
        while(i < 9){
            int new_clk = digitalRead(GPIOA, K_CLK);
            if (clk_state != new_clk){
                clk_state = new_clk;
                if (phase == 1){
                    i++;
                    press |= (digitalRead(GPIOA, K_DATA) << (i-1));
                }
                phase = !phase;
            }
        };
        //if (press == 0x40){
            //digitalWrite(GPIOA, SUCCESS_LED, 1);
        //}
    }

}
