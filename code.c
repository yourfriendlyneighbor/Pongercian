#include <Adafruit_SSD1306.h>

uint8_t ball_x = 64, ball_y = 16;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned int ball_update;

unsigned int paddle_update;
uint8_t cpu_y = 8;


uint8_t player_y = 8;

byte player1_Score = 0;
byte player2_Score = 0;

int rangedRand(const int & min, const int & max)
{
    int  x = ((rand() - min) % ((max - min) + 1));
    return  ((x < 0) ? x + max + 1 : x + min);
}

Adafruit_SSD1306 display(4);

void drawCourt();

void setup() {
    Serial.begin(9600);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    display.display(); // Optional
    unsigned long start = millis();

    pinMode(3, INPUT);
    pinMode(4, INPUT);

    display.clearDisplay();

    drawCourt();
    while(millis() - start < 2000);

    display.display();

    ball_update = millis();
    paddle_update = ball_update;
}

void loop() {
    const static unsigned int PROGMEM PADDLE_RATE = 33;
    const static unsigned int PROGMEM BALL_RATE = 16;  
    const static uint8_t PROGMEM PADDLE_HEIGHT = 12;

    const static uint8_t PROGMEM CPU_X = 12;
    const static uint8_t PROGMEM PLAYER_X = 115;
    bool update = false;
    unsigned long time = millis();

    static bool up_state = false;
    static bool down_state = false;
    
    up_state |= (digitalRead(3) == LOW);
    down_state |= (digitalRead(4) == LOW);

    if(time > ball_update) {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls
        if(new_x == 0 || new_x == 126) {
            //ball_dir_x = -ball_dir_x;
            //new_x += ball_dir_x + ball_dir_x;

            if(new_x == 126) {
              player1_Score = player1_Score + 1;
              display.clearDisplay();
              showScore();
            } else {
              player2_Score = player2_Score + 1;
              display.clearDisplay();
              showScore();
              askQuestion();
              showScore();
            }

            drawCourt();
            
            ball_dir_x = 1;
            ball_x = 64;
            ball_dir_y = 1;
            ball_y = 16;
            new_x = 65;
            new_y = 17;
            
          
        } else if(new_y == 0 || new_y == 31) { // Check if we hit the horizontal walls.
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
          
        } else if(new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {// Check if we hit the CPU paddle
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        } else if(new_x == PLAYER_X // Check if we hit the player paddle
           && new_y >= player_y
           && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        display.drawPixel(ball_x, ball_y, BLACK);
        display.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;

        update = true;
    }

    if(time > paddle_update) {
        paddle_update += PADDLE_RATE;

        // CPU paddle
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
        const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
        if(cpu_y + half_paddle -5 > ball_y- 6) {
            cpu_y -= 1;
        }
        if(cpu_y + half_paddle -5 < ball_y+6) {
            cpu_y += 1;
        }
        if(cpu_y < 1) cpu_y = 1;
        if(cpu_y + PADDLE_HEIGHT > 63) cpu_y = 31 - PADDLE_HEIGHT;
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

        // Player paddle
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
        if(up_state) {
            player_y -= 1;
        }
        if(down_state) {
            player_y += 1;
        }
        up_state = down_state = false;
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 63) player_y = 31 - PADDLE_HEIGHT;
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

        update = true;
    }

    if(update)
        display.display();
    
}

void showScore(void){
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(F("SCORE:"));
  display.setCursor(42.66667, 8.5);
  display.println(F("CPU"));
  display.setCursor(83, 8.5);
  display.println(F("PLAYER"));
  display.setCursor(42.66667,19);
  display.println(player1_Score);
  display.setCursor(83,19);
  display.println(player2_Score);

  display.display();

  delay(3000);

  display.clearDisplay();
}

void askQuestion(void){

  String QA;
  String QB;

  String question;
  String PossibleAnswer;
  String answer;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(0.25);
  int i = rangedRand(0,7);
  int position = rangedRand(1,2);
  
  if(i == 0) {
    question = "How many U.S. Senators are there?";
    PossibleAnswer = String(rangedRand(1, 99));
    answer = "100";
  } else if(i == 1) {
    question = "The name of the president of the USA?";
    PossibleAnswer = rangedRand(1,2) == 2 ? "George W Bush" : "Barack Obama";
    answer = "Donald Trump";
  } else if(i == 2) {
    question = "What is an ammendment";
    PossibleAnswer = rangedRand(1,2) == 1 ? "A thing you ammend" : "The Consitution";
    answer = "A Change or addition to the consitution";
  } else if(i == 3) {
    question = "What are the first ten ammendments called?";
    PossibleAnswer = rangedRand(1,2) == 1 ? "The Bill of Laws" : "The Consitution";
    answer = "The Bill of Rights";
  } else if(i == 4) {
    question = "What is the Supreme Law of the land?";
    PossibleAnswer = rangedRand(1,2) == 1 ? "The President" : "The Bill of Rights";
    answer = "The Consitution";
  } else if(i == 5) {
    question = "We elect a president for how many years?";
    PossibleAnswer = String(rangedRand(1, 20));
    answer = "4";
  } else if(i == 6) {
    question = "What are the two parts of the U.S Congress?";
    PossibleAnswer = rangedRand(1,2) == 1 ? "The Presidentary and The Supreme Court" : "The Democrats and the Republicans";
    answer = "A Change or addition to the consitution";
  } else if(i == 7) {
    question = "Who is charge in the executive branch?";
    PossibleAnswer = rangedRand(1,2) == 1 ? "The Leader of the House" : "The Senator";
    answer = "The President";
  }

  display.println(question);
  display.setCursor(0, 16);
  if(position == 1) {
   display.println(answer);
   display.setCursor(0, 24);
   QA = PossibleAnswer;
   QB = answer;
   display.println(PossibleAnswer);
  } else if(position == 2) {
   Serial.println(F("2"));
   display.println(answer);
   QA = PossibleAnswer;
   QB = answer;
   display.setCursor(0, 24);
   display.println(PossibleAnswer);
  }
  
 
  display.display();
  
  while (digitalRead(3) != HIGH or digitalRead(4) != HIGH) {
    if(digitalRead(3) == HIGH) {
      if(QA == answer) {
        // Success
        showStatus("Success");
        player2_Score = player2_Score + 1;
        break;
      } else {
        showStatus("Fail");
        player2_Score = player2_Score - 1;
        break;
        
      }
    }
    if(digitalRead(4) == HIGH) {
      if(QB == answer) {
        // Success
        showStatus("Success");
        player2_Score = player2_Score + 1;
        break;
      } else {
        showStatus("Fail");
        player2_Score = player2_Score - 1;
        break;
      }
    }
  }


  display.clearDisplay();
}

void showStatus(String status) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(status);
  display.display();
  delay(2000);
}

void drawCourt() {
    display.drawRect(0, 0, 128, 32, WHITE);
}
