
#include "mastermind.h"

void setup() {
  mastermind_init();
  Serial.begin(9600);
 
}

void loop(){
   
    char* code = generate_code(false, 4);
    play_game(code);
    free(code);
}

