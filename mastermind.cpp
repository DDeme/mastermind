#include <Arduino.h>
#include "lcd_wrapper.h"

#define BTN_1_PIN       2
#define BTN_2_PIN       3
#define BTN_3_PIN       4
#define BTN_4_PIN       5
#define BTN_ENTER_PIN   0

#define LED_BLUE_1      6
#define LED_RED_1       7
#define LED_BLUE_2      8
#define LED_RED_2       9
#define LED_BLUE_3      10
#define LED_RED_3       11
#define LED_BLUE_4      12
#define LED_RED_4       13


void mastermind_init() {
  lcd_init();

  // initialize the pushbutton pin as an input:
  pinMode(BTN_1_PIN, INPUT);
  pinMode(BTN_2_PIN, INPUT);
  pinMode(BTN_3_PIN, INPUT);
  pinMode(BTN_4_PIN, INPUT);
  pinMode(BTN_ENTER_PIN, INPUT);

  // initialize the LED pin as an output:
  pinMode(LED_BLUE_1, OUTPUT);
  pinMode(LED_RED_1, OUTPUT);
  pinMode(LED_BLUE_2, OUTPUT);
  pinMode(LED_RED_2, OUTPUT);
  pinMode(LED_BLUE_3, OUTPUT);
  pinMode(LED_RED_3, OUTPUT);
  pinMode(LED_BLUE_4, OUTPUT);
  pinMode(LED_RED_4, OUTPUT);
}

/**
   Generate secret code

   Generates secret code, which consists from digits only as a string.
   No two digits in the code combination can be same! Only if the repeat
   parameter is set to true.
   The length of generated code is set by the parameter length. If the given
   parameter is less than 1, function will generate no code and returns NULL.
   @param repeat if set to true, numbers in the generated code can repeat
   @param length the length of the code
   @return Pointer to the random generated code as string of given length. If length param is less than 1, return NULL.
*/
// char* generate_code(bool repeat, int length);
char* generate_code(bool repeat, int length) {
  if (length < 1  || (length > 10 && !repeat)) {
    return NULL;
  }

  char rand_code[length - 1];
  bool alreadyExists = true;
  int randnum;

  for (int i = 0; i < length; i++) {
    if (!repeat) {
      alreadyExists = true;

      while (alreadyExists) {
        randnum = random(9);
        alreadyExists = false;

        for (int n = 0; n < i && !alreadyExists; n++) {
          if (rand_code[n] == randnum) {
            alreadyExists = true;
          }
          else {
            alreadyExists = false;
          }
        }
      }

      rand_code[i] = randnum;
    }
    else {
      rand_code[i] = random(9);
    }
  }
  return rand_code;
}

/**
   Turns all RGB leds off
*/
void turn_off_leds() {
  digitalWrite(LED_BLUE_1, LOW);
  digitalWrite(LED_RED_1, LOW);
  digitalWrite(LED_BLUE_2, LOW);
  digitalWrite(LED_RED_2, LOW);
  digitalWrite(LED_BLUE_3 , LOW);
  digitalWrite(LED_RED_3, LOW);
  digitalWrite(LED_BLUE_4, LOW);
  digitalWrite(LED_RED_4, LOW);
}

/**
   Renders the RGB leds based on the guessed combination

   Lights up RGB leds with propper colors based on the current guessed combination.
   Red color is for guessed number on correct position and blue number is for
   guessed number on incorrect posititoin.
   @param peg_a number of guessed digits on correct position
   @param peg_b number of guessed digits on incorrect position
*/
// void render_leds(const int peg_a, const int peg_b);
void render_leds(const int peg_a, const int peg_b) {
  turn_off_leds();
  int led_num = 4;
  for (int i = 1; i <= led_num; i++) {
    if (i <= peg_a) {
      // zasvieť červeno
      switch (i) {
        case 1:
          digitalWrite(LED_RED_1, HIGH);
          break;
        case 2:
          digitalWrite(LED_RED_2, HIGH);
          break;
        case 3:
          digitalWrite(LED_RED_3, HIGH);
          break;
        case 4:
          digitalWrite(LED_RED_4, HIGH);
          break;
      }
    }
    else if ( (i - peg_a) <= peg_b ) {
      // zasvieť modro
      switch (i) {
        case 1:
          digitalWrite(LED_BLUE_1, HIGH);
          break;
        case 2:
          digitalWrite(LED_BLUE_2, HIGH);
          break;
        case 3:
          digitalWrite(LED_BLUE_3, HIGH);
          break;
        case 4:
          digitalWrite(LED_BLUE_4, HIGH);
          break;
      }
    }
  }
}

/**
   Renders history

   Renders line of a history defined by the parameter entry_nr on the display.
   @param secret reference to the secret combination
   @param history list (array) of history entries
   @param entry_nr index of history entry to render
*/
//void render_history(char* secret, char** history, const int entry_nr);
void render_history(char* secret, char** history, const int entry_nr) {
  lcd_clear();
  lcd_print_at(0, 1, char(entry_nr + 1));
  lcd_print_at(2, 1, ": ");
  lcd_print_at(4, 1, history[entry_nr][0]);
  lcd_print_at(10, 1, history[entry_nr][1]);
  lcd_print_at(11, 1, "A");
  lcd_print_at(12, 1, history[entry_nr][1]);
  lcd_print_at(13, 1, "B");
  render_leds(history[entry_nr][1], history[entry_nr][2]);
}


/**
   Returns score for the given guess

   @param secret the pointer to the secret code
   @param guess the pointer to the current guess
   @param peg_a output parameter to get number of As for current guess
 * * @param peg_b output parameter to get number of Bs for current guess
*/
// void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b);
void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b) {
  int lengthOfSecret = strlen(secret);
  int lengthOfGuess = strlen(guess);
  *peg_a = 0;
  *peg_b = 0;
  for (int i = 0; i < lengthOfSecret; i++) {
    if (secret[i] == guess[i]) {
      (*peg_a)++;
    }
    else {
      for (int n = 0; n < lengthOfGuess; n++) {
        if (secret[i] == guess[n]) {
          (*peg_b)++;
        }
      }
    }
  }
}

/**
   Game loop

   This is the main game loop. Function is called with the secret combination and
   ends, when the combination is found or when the player runs out of the attempts.
   @param secret reference to the secret combination
*/
// void play_game(char* secret);
void play_game(char* secret) {
  char** history;
  char* guess;
  int currentTry = 0;
  int peg_a;
  int peg_b;
  int currentHistory = 0;
  char** message;

  // set the guess
  guess[0] = 0;
  guess[1] = 1;
  guess[2] = 2;
  guess[3] = 3;

  turn_off_leds();
  lcd_clear();
  


  lcd_print_at(0, 1, "Welcome to MasterMind");
  lcd_print_at(0, 2, "Your goal is to guess my secret combination.");

  // WAIT TILL ENTER IS PRESS TO START THE GAME
  while (true) {
    delay(100);
    if (digitalRead(BTN_ENTER_PIN) == HIGH) {
      break;
    }
  }

  lcd_clear();
  lcd_print_at(0, 1, "I am thinking a number:");
  lcd_print_at(0, 2, "Your guess:");
  lcd_print_at(12, 2, guess[0]);
  lcd_print_at(13, 2, guess[1]);
  lcd_print_at(14, 2, guess[2]);
  lcd_print_at(15, 2, guess[3]);

  while (true) {
  
    // incrementing guess
    if (digitalRead(BTN_1_PIN) == HIGH &&
        digitalRead(BTN_2_PIN) == LOW &&
        digitalRead(BTN_3_PIN) == LOW &&
        digitalRead(BTN_4_PIN) == LOW) {
      if (guess[0] == 9) {
        guess[0] = 0;
      }
      else {
        guess[0]++;
      }
      lcd_print_at(12, 2, guess[0]);
      lcd_print_at(13, 2, guess[1]);
      lcd_print_at(14, 2, guess[2]);
      lcd_print_at(15, 2, guess[3]);
    }

    if (digitalRead(BTN_1_PIN) == LOW &&
        digitalRead(BTN_2_PIN) == HIGH &&
        digitalRead(BTN_3_PIN) == LOW &&
        digitalRead(BTN_4_PIN) == LOW) {
      if (guess[1] == 9) {
        guess[1] = 0;
      }
      else {
        guess[1]++;
      }
      lcd_print_at(12, 2, guess[0]);
      lcd_print_at(13, 2, guess[1]);
      lcd_print_at(14, 2, guess[2]);
      lcd_print_at(15, 2, guess[3]);
    }

    if (digitalRead(BTN_1_PIN) == LOW &&
        digitalRead(BTN_2_PIN) == LOW &&
        digitalRead(BTN_3_PIN) == HIGH &&
        digitalRead(BTN_4_PIN) == LOW) {
      if (guess[2] == 9) {
        guess[2] = 0;
      }
      else {
        guess[2]++;
      }
      lcd_print_at(12, 2, guess[0]);
      lcd_print_at(13, 2, guess[1]);
      lcd_print_at(14, 2, guess[2]);
      lcd_print_at(15, 2, guess[3]);
    }

    if (digitalRead(BTN_1_PIN) == LOW &&
        digitalRead(BTN_2_PIN) == LOW &&
        digitalRead(BTN_3_PIN) == LOW &&
        digitalRead(BTN_4_PIN) == HIGH) {
      if (guess[3] == 9) {
        guess[3] = 0;
      }
      else {
        guess[3]++;
      }
      lcd_print_at(12, 2, guess[0]);
      lcd_print_at(13, 2, guess[1]);
      lcd_print_at(14, 2, guess[2]);
      lcd_print_at(15, 2, guess[3]);
    }

    // HISTORY
    if (digitalRead(BTN_1_PIN) == HIGH &&
        digitalRead(BTN_2_PIN) == HIGH &&
        digitalRead(BTN_3_PIN) == HIGH &&
        digitalRead(BTN_4_PIN) == LOW &&
        currentHistory > 0) {
      currentHistory--;
      render_history(secret, history, currentHistory);
    }

    if (digitalRead(BTN_1_PIN) == HIGH &&
        digitalRead(BTN_2_PIN) == HIGH &&
        digitalRead(BTN_3_PIN) == LOW &&
        digitalRead(BTN_4_PIN) == HIGH &&
        currentHistory < (currentTry - 1)) {
      currentHistory++;
      render_history(secret, history, currentHistory);
    }

    // play next game after end
    if (digitalRead(BTN_ENTER_PIN) == HIGH && (peg_a == 4 || currentTry == 10) ) {
      turn_off_leds();
      break;
    }   // move to next try
    else if (digitalRead(BTN_ENTER_PIN) == HIGH) {
      get_score(secret, guess, &peg_a, &peg_b);
      currentHistory = currentTry;
      history[currentHistory][0] = guess;
      history[currentHistory][1] = peg_a;
      history[currentHistory][2] = peg_b;

      render_history(secret, history, currentHistory);
      currentTry++;
      if (peg_a == 4) {
        lcd_clear();
        lcd_print_at(0, 2, "Well done! You win!");
      }
      else if (currentTry == 10) {
        lcd_clear();
        lcd_print_at(0, 2, "You loose! Try again.");
      }
    }

    // delay for multiple move press of buttons
    delay(1000);
  }
}
