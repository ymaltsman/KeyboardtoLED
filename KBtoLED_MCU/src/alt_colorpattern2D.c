

#include <stdio.h>
#include <math.h>
#include "STM32F401RE.h"
#include "main.h"


////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////

#define N 7



#define BASE_COL 100 //base color




void process_array_color(uint8_t color[3], uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
void init_LED(uint8_t LED[N][3], uint8_t color[3]);
void init_2DLED(uint8_t LED[M][N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);
void sendLEDmatrix(uint8_t LED[M][N][3]);
void process_matrix(uint8_t LED[M][N][3], uint8_t LED0[M][N][3], int x, float t);
double wave_function(int x, int x0, float t, float c);
int keytoloc(key keys[K], uint8_t data);
uint8_t getkey();
void init_keys(key keys[K]);
uint8_t fetch_data();
void getrand(uint8_t color[3]);
process_matrix_color(uint8_t color[3], uint8_t LED[M][N][3], uint8_t LED0[M][N][3], int x0,float t);
process_matrix_color2(uint8_t color[3], uint8_t LED[M][N][3], uint8_t LED0[M][N][3], int x0,float t);
uint8_t getsignal();


int main(void){
        // Configure flash latency and set clock to run at 84 MHz
  configureFlash();
  configureClock();

  key keys[K];
  init_keys(keys);

  // Enable GPIOA clock
  RCC->AHB1ENR.GPIOAEN = 1;
  RCC->AHB1ENR.GPIOCEN = 1;

  // "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
  spiInit(1, 0, 0);


  // Load pin
  pinMode(GPIOB, LOAD_PIN, GPIO_OUTPUT);
  //pinMode(GPIOB, K_CLK, GPIO_INPUT);
  //pinMode(GPIOA, K_DATA, GPIO_INPUT);
  pinMode(GPIOA, FPGA_FLAG, GPIO_INPUT);
  pinMode(GPIOA, Ready_PIN, GPIO_OUTPUT);
  digitalWrite(GPIOA, Ready_PIN, 0);


  //initialize LED array
  uint8_t LED0[M][N][3];
  uint8_t LED[M][N][3];
  uint8_t color[3] = {0xFF, 0xFF, 0xFF};

  float t = 0;
  int x0;
  float dt = 0.01;
  float end = 500;

  init_2DLED(LED0, color);
  init_2DLED(LED, color);
  sendLEDmatrix(LED);
  uint8_t data;
  
  while(1){
      if (t == 0 || t > 2){
          if (digitalRead(GPIOA, FPGA_FLAG)){
                data = getsignal();
                x0 = keytoloc(keys, data);
                t = .01;
                getrand(color);
          }
      }
/*
    if (t < .01){
          sendLEDmatrix(LED);
    }
    else if (t >= end){
        t = 0;
    }
*/    
    if (t >= 0.009 && t <= end) {
        process_matrix_color(color, LED, LED0, x0, t);
        sendLEDmatrix(LED);
        t += dt;
        if (t == end){
            t = 0;
        }
    }
    


  }

}

uint8_t getsignal(){
    //Conduct SPI transaction with FPGA to receive keypress data
    uint8_t data;

    digitalWrite(GPIOA, Ready_PIN, 1);
    data = spiSendReceive(0);
    while(SPI1->SR.BSY);
    digitalWrite(GPIOA, Ready_PIN, 0);
    //data = data << 1; //left shift by 1 to make up for weird right shifting
    return data;
}


process_matrix_color2(uint8_t color[3], uint8_t LED[M][N][3],uint8_t LED0[M][N][3],int x0,float t){
    /*
    Applies a pattern to the LED matrix, where the pattern is two diverging travelling waves.
    */
    int i;
    int j;
    int x;
    float c = 3;
    for (j = 0; j < M; j++){
        for (i = 0; i < N; i++){
            x = i + N*j;
            LED[j][i][0] = 2*LED0[j][i][0] - (wave_function(x, x0, t, c) + wave_function(x, x0, t, -c))*color[0];
            LED[j][i][1] = 2*LED0[j][i][1] - (wave_function(x, x0, t, c) + wave_function(x, x0, t, -c))*color[1];
            LED[j][i][2] = 2*LED0[j][i][2] - (wave_function(x, x0, t, c) + wave_function(x, x0, t, -c))*color[2];

        }
    }
    
}


process_matrix_color(uint8_t color[3], uint8_t LED[M][N][3],uint8_t LED0[M][N][3],int x0,float t){
    /*
    Applies a pattern to the LED matrix, where the pattern is a single travelling wave.
    */
    int i;
    int j;
    int x;
    float c = 3;
    for (j = 0; j < M; j++){
        for (i = 0; i < N; i++){
            x = i + N*j;
            LED[j][i][0] = 2*LED0[j][i][0] - wave_function(x, x0, t, c)*color[0];
            LED[j][i][1] = 2*LED0[j][i][1] - wave_function(x, x0, t, c)*color[1];
            LED[j][i][2] = 2*LED0[j][i][2] - wave_function(x, x0, t, c)*color[2];

        }
    }
    
}


void process_matrix(uint8_t LED[M][N][3], uint8_t LED0[M][N][3], int x0, float t){
    /*
    Applies a pattern to the LED matrix, where the pattern is two diverging travelling waves (no random color).
    */
    int i;
    int j;
    int x;
    for (j = 0; j < M; j++){
        for (i = 0; i < N; i++){
            x = i + N*j;
            LED[j][i][0] = LED0[j][i][0] - wave_function(x, x0, t, 3)*BASE_COL;
            LED[j][i][1] = LED0[j][i][1] - wave_function(x, x0, t, 3)*BASE_COL;
            LED[j][i][2] = LED0[j][i][2] - wave_function(x, x0, t, 3)*BASE_COL;

        }
    }

}


void process_array_color(uint8_t color[3], uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t){
    int i;
    for (i = 0; i < N; i++){
        LED[i][0] = LED0[i][0] - wave_function(i, x, t, 3)*color[0];
        LED[i][1] = LED0[i][1] - wave_function(i, x, t, 3)*color[1];
        LED[i][2] = LED0[i][2] - wave_function(i, x, t, 3)*color[2];
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
    keys[0].data = 0x07;
    keys[0].loc = 1;

    keys[1].data = 0x0C;
    keys[1].loc = 8;
}

void init_2DLED(uint8_t LED[M][N][3], uint8_t color[3]){ 
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

void init_LED(uint8_t LED[N][3], uint8_t color[3]){ 
    //initialize LED array to be the same color
    int i;

    for (i = 0; i < N; i++){
        LED[i][0] = color[0];
        LED[i][1] = color[1];
        LED[i][2] = color[2];
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


double wave_function(int x, int x0, float t, float c){
    double w = 1;
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
        LED[i][0] = LED0[i][0] - wave_function(i, x, t, 3)*BASE_COL;
        LED[i][1] = LED0[i][1] - wave_function(i, x, t, 3)*BASE_COL;
        LED[i][2] = LED0[i][2] - wave_function(i, x, t, 3)*BASE_COL;
    }
}

