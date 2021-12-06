#ifndef STM32F4_functions_H
#define STM32F4_functions_H

#include <stdint.h>

#define N 7
#define M 2
#define K 2


typedef struct {
  union { 
    struct {
      unsigned int start : 1;
      unsigned int data : 8;
      unsigned int parity : 1;
      unsigned int stop : 1;
    };
    int raw;
  };
} ps2_frame_t;

typedef struct {
  unsigned int data : 8;
  unsigned int loc  : 8;
  unsigned int color  : 8;
} key;

typedef struct {
  unsigned int loc  : 8;
  float t;
  unsigned int green  : 8;
  unsigned int red  : 8;
  unsigned int blue : 8; 
} press;

void process_array_color(uint8_t color[3], uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
void process_array(uint8_t LED[N][3], uint8_t LED0[N][3], int x, float t);
void init_LED(uint8_t LED[M][N][3], uint8_t color[3]);
void sendLEDarray(uint8_t LED[N][3]);
void sendLEDmatrix(uint8_t LED[M][N][3]);
void process_matrix(uint8_t LED[M][N][3], uint8_t LED0[M][N][3], int x, float t);
double wave_function(int x, int x0, float t);
int keytoloc(key keys[K], uint8_t data);
uint8_t getkey();
void init_keys(key keys[K]);
uint8_t fetch_data();
void getrand(uint8_t color);

#endif