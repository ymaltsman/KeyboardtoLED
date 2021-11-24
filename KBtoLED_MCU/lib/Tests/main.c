#include <stdio.h>
#include <math.h>

//#include "STM32F401RE.h"

#define _USE_MATH_DEFINES
#define N 5
#define BASE_COL 100
//function that initializes LED array
//    -->send this to the FPGA via SPI
//    -->Store current state of array, with each while loop pass
//    this and any keypresses into function that outputs next array
//    --> something for processing array into LED readable format

void init_LED(uint8_t LED[N][3], uint8_t color[3]){ 
    //initialize LED array to be the same color
    int i;

    for (i = 0; i < N; i++){
        LED[i][0] = color[0];
        LED[i][1] = color[1];
        LED[i][2] = color[2];
    }

}

double wave_function(int x, int x0, int t){
    double u;
    double k = (x-x0)*(x-x0);
    u = (1/sqrt(4*M_PI*t))*exp(-k/(4*t));
    return u;
}

void process_array(uint8_t LED[N][3], int x, int t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = wave_function(i, x, t)*BASE_COL;
        LED[i][1] = wave_function(i, x, t)*BASE_COL;
        LED[i][2] = wave_function(i, x, t)*BASE_COL;
    }
}

int main(void){

    //configureFlash();
    //configureClock();
    

    //for testing
    uint8_t LED[N][3];

    uint8_t color[3] = {100, 100, 100};
    init_LED(LED, color);

    int x = 2;
    int t = 1;
    process_array(LED, x, t);


    while(1){
        //when a key is pressed, we set t = 0
        //process_array(LED);
        //t += 1;


    }
}