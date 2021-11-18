//includes
#include <stdio.h>
#include "STM32F401RE.h"
//strip length
#define N 5 
//array with keystrokes, location on LED array
int keys_LED[N][2] = {
    {48, 0},
    {49, 1},
    {50, 2},
    {51, 3},
    {52, 4}
};

//hash table
struct Key_to_LED {
    int keyp;
    int LED;
    UT_hash_handle hh;
}

//need to declare hash as null pointer
struct Key_to_LED *KLptr = NULL;

void add_LED(int keyp, int LED) {
    struct Key_to_LED *s;

    s = malloc(sizeof(struct Key_to_LED));
    s->keyp = keyp;
    s->LED = LED;
    HASH_ADD_INT(KLptr, keyp, s); 
}

struct Key_to_LED *find_LED(int keyp) {
    struct Key_to_LED *s;

    HASH_FIND_INT(KLptr, &keyp, s);  /* s: output pointer */
    return s;
}

add_LED(Keys_LED[0][0], Keys_LED[0][1]);
find_LED(48);
