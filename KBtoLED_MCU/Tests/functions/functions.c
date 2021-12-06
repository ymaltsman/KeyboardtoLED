#include "functions.h"

void process_matrix(uint8_t LED[M][N][3],uint8_t LED0[M][N][3],int x0,float t){
    int i;
    int j;
    int x;
    for (j = 0; j < M; j++){
        for (i = 0; i < N; i++){
            x = i + N*j;
            LED[j][i][0] = LED0[j][i][0] - wave_function(x, x0, t)*BASE_COL;
            LED[j][i][1] = LED0[j][i][1] - wave_function(x, x0, t)*BASE_COL;
            LED[j][i][2] = LED0[j][i][2] - wave_function(x, x0, t)*BASE_COL;

        }
    }

}

void process_array_color(uint8_t color[3], uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = LED0[i][0] - wave_function(i, x, t)*color[0];
        LED[i][1] = LED0[i][1] - wave_function(i, x, t)*color[1];
        LED[i][2] = LED0[i][2] - wave_function(i, x, t)*color[2];
    }
}

void getrand(uint8_t color[3]){
    int lower = 0;
    int upper = 255;
    int i;
    for (i = 0; i<3; i++){
        color[i] = (rand() % (upper - lower + 1) + lower);
    }
}

void init_keys(key keys[K]){
    keys[0].data = 0x1C;
    keys[0].loc = 0;

    keys[1].data = 0x32;
    keys[1].loc = 1;
}

void init_LED(uint8_t LED[M][N][3], uint8_t color[3]){ 
    //initialize LED array to be the same color
    int i;
    int j;
    for (j = 0; j< M; j++){
        for (i = 0; i < N; i++){
            LED[j][i][0] = color[0];
            LED[j][i][1] = color[1];
            LED[j][i][2] = color[2];
        }

    }

}

void sendLEDmatrix(uint8_t LED[M][N][3]){
    int k;
    digitalWrite(GPIOB, LOAD_PIN, 1);
    for (k = 0; k< M; k++){
        sendLEDarray(LED[k]);
    }
    digitalWrite(GPIOB, LOAD_PIN, 0);
}

void sendLEDarray(uint8_t LED[N][3]){
    int i;
    //uint8_t send;
    int j;
    //digitalWrite(GPIOB, LOAD_PIN, 1);
    for(i = 0; i<N; i++){
        for(j = 0; j<3; j++){
            
            spiSendColor(LED[i][j]);
            while(SPI1->SR.BSY); // Confirm all SPI transactions are completed
            
        }
    }
    //digitalWrite(GPIOB, LOAD_PIN, 0);
    
}

uint8_t fetch_data(){
    int i;
    ps2_frame_t ps2_frame;
    for (i = 0; i < 11; i++){
        ps2_frame.raw |= digitalRead(GPIOA, K_DATA) << i;
        while(!digitalRead(GPIOB, K_CLK));
        while(digitalRead(GPIOB, K_CLK));
    }
    return ps2_frame.data;
}


double wave_function(int x, int x0, float t){
    double w = 1;
    double c = 3;
    double k = (x-c*t-x0)*(x-c*t-x0);
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

void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = LED0[i][0] - wave_function(i, x, t)*BASE_COL;
        LED[i][1] = LED0[i][1] - wave_function(i, x, t)*BASE_COL;
        LED[i][2] = LED0[i][2] - wave_function(i, x, t)*BASE_COL;
    }
}
