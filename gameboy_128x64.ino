/**************************************************************************
Vectozavr - YouTube channel about science, experiments and programming.
You can share, modify and use this code for any purpose you want :)
 **************************************************************************/
 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <time.h>
#include <string.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define pinX    A2  // ось X джойстика
#define pinY    A1  // ось Y джойстика
#define swPin    2  // кнопка джойстика
#define ledPin  13  // светодиод на Pin 13

struct Point2D {
  float x = 0;
  float y = 0;
};

struct Point2D_int8_t {
  int8_t x = 0;
  int8_t y = 0;
};

struct Segment {
  Point2D p1;
  Point2D p2;
};

typedef struct obstacle {
  int slotSize = 37;
  int h = 0;
  int w = 8;
  float x = SCREEN_WIDTH + w/2;
};

struct JoyStick {
  uint32_t t_last_pressed = millis();
  int STEPS = 3968;

  bool button () {
    boolean ledState = digitalRead(swPin);
    digitalWrite(ledPin, !ledState);

    double diff = (millis() - t_last_pressed);
    if(!ledState && diff > 1000) {
      t_last_pressed = millis();
      return true;
    }
    return false;
  }

  float x() {
    int X = analogRead(pinX);

    if(abs(STEPS/2 - X) > STEPS/3)
      return( -2.0f * (STEPS/2 - X) / STEPS );
    return 0;
  }

  float y() {
    int Y = analogRead(pinY);
    
    if(abs(STEPS/2 - Y) > STEPS/3)
      return( -2.0f * (STEPS/2 - Y) / STEPS );
    return 0;
  }
};

JoyStick joyStick;

void menu(int game = 0) {
  
  uint32_t t_last_select = millis();

  int selectedGame = game;

  while(true) {

    draw_menu_state(selectedGame);

    double diff = (millis() - t_last_select);
    double j = joyStick.x();
    Serial.println(analogRead(pinY));
    if(diff > 300) {
      if(abs(j) > 0.01) {
        selectedGame = j > 0 ? ((3 + selectedGame-1) % 3) : ((selectedGame+1) % 3);
        t_last_select = millis();
      }
    }
    
    if(joyStick.button())
      break;
  }

  switch(selectedGame) {
    case 0:
      fps();
      break;
    case 1:
      snake();
      break;
    case 2:
      flappyBird();
      break;
  }

  return;
}

static const unsigned char PROGMEM logo_bmp[] =
{ B00000011, B11000000,
  B00011100, B00111000,
  B00100000, B00000100,
  B01000000, B00000010,
  B10011110, B01111001,
  B10100001, B10001111,
  B11000011, B10001111,
  B11001111, B10011111,
  B10001111, B11011111,
  B10000110, B11011011,
  B11000001, B10000011,
  B11110011, B11000111,
  B01111111, B11111110,
  B00111111, B11111100,
  B00011111, B11111000,
  B00000011, B11000000 };

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
  
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  draw_logo();
  display.invertDisplay(true);
  delay(1500);
  display.invertDisplay(false);
  display.clearDisplay();
  display.display();

  menu();
  return;
}

void loop() {
  
}

void draw_menu_state(int selectedGame) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  switch(selectedGame) {
    case 0:
      display.setTextSize(2);
      display.setCursor(LOGO_WIDTH/2 + 15, LOGO_HEIGHT/2);
      display.println(F("3D game"));
      display.setTextSize(1);
      display.setCursor(LOGO_WIDTH/2 + 39, LOGO_HEIGHT/2 + 25);
      display.println(F("Snake"));
      display.setCursor(LOGO_WIDTH/2 + 35, LOGO_HEIGHT/2 + 40);
      display.println(F("Flappy"));
      break;
    case 1:
      display.setTextSize(1);
      display.setCursor(LOGO_WIDTH/2 + 33, LOGO_HEIGHT/2);
      display.println(F("3D game"));
      display.setTextSize(2);
      display.setCursor(LOGO_WIDTH/2 + 25, LOGO_HEIGHT/2 + 13);
      display.println(F("Snake"));
      display.setTextSize(1);
      display.setCursor(LOGO_WIDTH/2 + 35, LOGO_HEIGHT/2 + 40);
      display.println(F("Flappy"));
      break;
    case 2:
      display.setTextSize(1);
      display.setCursor(LOGO_WIDTH/2 + 33, LOGO_HEIGHT/2);
      display.println(F("3D game"));
      display.setCursor(LOGO_WIDTH/2 + 39, LOGO_HEIGHT/2 + 15);
      display.println(F("Snake"));
      display.setTextSize(2);
      display.setCursor(LOGO_WIDTH/2 + 18, LOGO_HEIGHT/2 + 30);
      display.println(F("Flappy"));
      break;
  }
   
  display.display();
  return;
}

void draw_logo(void) {
  
  display.drawBitmap((display.width()  - LOGO_WIDTH ) / 2, (display.height() - LOGO_HEIGHT) / 2, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(LOGO_WIDTH/2 + 30, LOGO_HEIGHT/2 + 35);
  display.println(F("Vectozavr"));
  
  display.display();
  return;
}

void fps(void) {  
  
  uint32_t tp = millis();
  
  // Camera parameters
  uint8_t SEGMENTS = 32;
  float f_depth = 12;
  float f_fov = PI/2;

  // Player parameters
  Point2D P_position;
  P_position.x = 4.0f;
  P_position.y = 3.0f;
  float f_dir = 0;
  float f_walk_speed = 1.0f;
  float f_view_speed = 2.0f;
  
  char map1[64];
  strcpy(map1, "########");
  strcat(map1, "# #  # #");
  strcat(map1, "# #  # #");
  strcat(map1, "#      #");
  strcat(map1, "# ##   #");
  strcat(map1, "#   ## #");
  strcat(map1, "#      #");
  strcat(map1, "########");

  while(true) {
    // time control
    double diff = (millis() - tp);
    tp = millis();
    
    display.clearDisplay();

    // ray-cast
    for(uint8_t i = 0; i < SEGMENTS; i++) {
      
      //float len = ray_cast_distance(map1, 8, 8, &P_position, f_dir + (float)f_fov*(i - SEGMENTS/2)/SEGMENTS, f_depth);
      
      //---------------------------------
      // ray cast
      float len = 0.0001f;
      float dx = 0.5f;
      float dx_min = 0.05f;
      
      
      while(true) {
      
        int ray_x = (int)(P_position.x + len * cos(f_dir + (float)f_fov*(i - SEGMENTS/2)/SEGMENTS) + 0.5);
        int ray_y = (int)(P_position.y + len * sin(f_dir + (float)f_fov*(i - SEGMENTS/2)/SEGMENTS) + 0.5);
      
        if(ray_x >= 0 && ray_x < 8 && ray_y >= 0 && ray_y < 8) {
          if(map1[8*ray_y + ray_x] == '#') {
            if(dx > dx_min) {
              len -= dx;
              dx /= 2;
            } else {
              break;
            }
          }
        }
        
        len += dx;
        if(len >= f_depth) {
          break;
          len = f_depth;
        }
      }
      
      //---------------------------------
      
      if(len >= f_depth)
        continue;

      int h0 = (1 - 1 / len) * SCREEN_HEIGHT / 2;
      int h1 = (1 + 1 / len) * SCREEN_HEIGHT / 2;
      
      if(SEGMENTS == SCREEN_WIDTH) {
        display.drawLine(i, h0, i, h1, SSD1306_WHITE); 
      } else {
        display.fillRect(i*SCREEN_WIDTH/SEGMENTS, h0, SCREEN_WIDTH/SEGMENTS, h1-h0, SSD1306_WHITE);
      }
    }
    
    // frame per seconds
    uint8_t fps = (int) 1000.0f / diff;
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);       
    display.setCursor(0, 0);
    display.print("[");
    display.print(P_position.x);
    display.print(", ");
    display.print(P_position.y);
    display.print("] : ");
    display.print(fps);
    display.println(F(" fps"));
    
    display.display();

    int STEPS = 3968;

    float f_dx = f_walk_speed * diff/1000 * cos(f_dir) * joyStick.x();
    float f_dy = f_walk_speed * diff/1000 * sin(f_dir) * joyStick.x();

    P_position.x += f_dx;
    P_position.y += f_dy;

    int i_x = roundf(P_position.x);
    int i_y = roundf(P_position.y);

    if(map1[8*i_y + i_x] == '#') {
      P_position.x -= f_dx;
      P_position.y -= f_dy;
    }
    
    f_dir += f_view_speed * diff/1000 * joyStick.y();
    
    // end the game
    if(joyStick.button())
      break;
  }

  menu(0);
  return;
}

void snake(void) {

  uint32_t tp = millis();
  uint32_t timer = 500;
  
  int8_t w = 16;
  int8_t h = 6;

  int cellSize = SCREEN_WIDTH / w;

  int8_t snailSize = 1;
  int8_t dir = 1;
  Point2D_int8_t vec_Snake[w*h];
  Point2D_int8_t P_food = {(int8_t)w/2 + 2, (int8_t)h/2 + 2};

  vec_Snake[0] = {(int8_t)w/2, (int8_t)h/2};

  bool won = false;

  while(true) {
    
    float diff = (millis() - tp);

    float dx = joyStick.x();
    float dy = joyStick.y();

    if(won) {
      dx = 0;
      dy = 0;
    }

    if(dx*dx + dy*dy > 0) {
      if(dx*dx > 0.25) {
        if(dx > 0) {
          if(dir != 3 && dir != 1) {
            dir = 1;
            diff = timer;
          }
        } else {
          if(dir != 1 && dir != 3) {
            dir = 3;
            diff = timer;
          }
        }
      } else if(dy*dy > 0.25) {
        if(dy > 0) {
          if(dir != 4 && dir != 2) {
            dir = 2;
            diff = timer;
          }
        } else {
          if(dir != 2 && dir != 4) {
            dir = 4;
            diff = timer;
          }
        }
      }
    }
    
    if(diff >= timer && !won) {
      display.clearDisplay();
      tp = millis();

      if(vec_Snake[0].x == P_food.x && vec_Snake[0].y == P_food.y) {
        snailSize++;
        P_food.x = (int8_t)(1 + (w-1)*(float)rand()/RAND_MAX);
        P_food.y = (int8_t)(1 + (h-1)*(float)rand()/RAND_MAX);
      }
      
      for(int8_t i = snailSize-1; i > 0; i--) {
        if(vec_Snake[i].x == vec_Snake[0].x && vec_Snake[i].y == vec_Snake[0].y) {
          won = true;
          continue;
        }
        
        vec_Snake[i].x = vec_Snake[i-1].x;
        vec_Snake[i].y = vec_Snake[i-1].y;
      }
      
      switch(dir) {
        case 1:
          vec_Snake[0].y++;
          break;
        case 2:
          vec_Snake[0].x++;
          break;
        case 3:
          vec_Snake[0].y--;
          break;
        case 4:
          vec_Snake[0].x--;
          break;
      }

      for(int i = 0; i < snailSize; i++) {
        if(vec_Snake[i].y < 1)
          vec_Snake[i].y = h;
        if(vec_Snake[i].y > h)
          vec_Snake[i].y = 1;

        if(vec_Snake[i].x < 0)
          vec_Snake[i].x = w-1;
        if(vec_Snake[i].x > w-1)
          vec_Snake[i].x = 0;
        display.fillRect(vec_Snake[i].x * cellSize, SCREEN_HEIGHT - vec_Snake[i].y * cellSize, cellSize, cellSize, SSD1306_WHITE);
      }

      display.fillRect(P_food.x * cellSize, SCREEN_HEIGHT - P_food.y * cellSize, cellSize, cellSize, SSD1306_WHITE);

      display.fillRect(0, 0, 128, 16, SSD1306_WHITE);

      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK);       
      display.setCursor(4, 4);
      display.print("score: ");
      display.print(snailSize);
    
      display.display();
    }
    
    if(joyStick.button())
      break;
  }

  menu(1);
  return;
}

void drawObstacle(obstacle& obs) {
  /*  uint16_t x = SCREEN_WIDTH + w/2;
   *  uint8_t slotSize = 10;
   *  uint8_t h = 0;
   *  uint8_t w = 4;
   */
  display.fillRect(obs.x - obs.w/2 + 2, 0, obs.w - 4, obs.h, SSD1306_WHITE);
  display.fillRect(obs.x - obs.w/2 + 2, obs.h + obs.slotSize, obs.w - 4, SCREEN_HEIGHT - obs.h - obs.slotSize, SSD1306_WHITE);

  display.fillRect(obs.x - obs.w/2, obs.h - 4, obs.w, 4, SSD1306_WHITE);
  display.fillRect(obs.x - obs.w/2, obs.h + obs.slotSize, obs.w, 4, SSD1306_WHITE);
}

void flappyBird(void) {

  uint32_t tp = millis();

  float o_speed = 0.05;

  float bird_y = 0.5;
  float bird_vy = 0;
  float g = -1;

  uint8_t birdSize = 3;

  obstacle obs1;
  obstacle obs2;

  obs2.x += SCREEN_WIDTH/2;

  int score = 0;

  bool won = false;
  
  while(true) {

    float dx = joyStick.x();
    float dy = joyStick.y();

    if(won) {
      dx = 0;
      dy = 0;
    }
    
    if (dx*dx + dy*dy > 0.5)
    {
      //jump
      bird_vy = 0.5;
    }
    
    float diff = (millis() - tp);
    tp = millis();

    bird_vy += diff * g / 500;
    bird_y += diff * bird_vy / 500;

    if(bird_y > 1) {
      bird_y = 1;
      bird_vy = 0;
    }
    if(bird_y < 0) {
      bird_y = 0;
      bird_vy = 0;
    }

    if(!won) {
      obs1.x -= diff * o_speed;
      obs2.x -= diff * o_speed;
    }

    if(obs1.x < -obs1.w/2) {
      obs1.x = SCREEN_WIDTH + obs1.w/2;
      obs1.h = (SCREEN_HEIGHT - obs1.slotSize)*(float)rand()/RAND_MAX;
      score++;
    }

    if(obs2.x < -obs2.w/2) {
      obs2.x = SCREEN_WIDTH + obs2.w/2;
      obs2.h = (SCREEN_HEIGHT - obs2.slotSize)*(float)rand()/RAND_MAX;
      score++;
    }

    if((obs1.x > 30  - LOGO_WIDTH / 2) && (obs1.x < 30  + LOGO_WIDTH / 2)) {
      bool collision = (bird_y*SCREEN_HEIGHT - LOGO_HEIGHT / 2 < SCREEN_HEIGHT-obs1.h-obs1.slotSize) || (bird_y*SCREEN_HEIGHT + LOGO_HEIGHT / 2 > SCREEN_HEIGHT-obs1.h);
      if(collision)
        won = true;
    }

    if((obs2.x > 30  - LOGO_WIDTH / 2) && (obs2.x < 30  + LOGO_WIDTH / 2)) {
      bool collision = (bird_y*SCREEN_HEIGHT - LOGO_HEIGHT / 2 < SCREEN_HEIGHT-obs2.h-obs2.slotSize) || (bird_y*SCREEN_HEIGHT + LOGO_HEIGHT / 2 > SCREEN_HEIGHT-obs2.h);
      if(collision)
        won = true;
    }

    display.clearDisplay();
    
    //display.drawCircle(30, SCREEN_HEIGHT*(1 - bird_y), birdSize, SSD1306_WHITE);

    display.drawBitmap(30  - LOGO_WIDTH / 2, SCREEN_HEIGHT*(1 - bird_y) - LOGO_HEIGHT / 2, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.fillRect(obs1.x - obs1.w/2 + 2, 0, obs1.w - 4, obs1.h, SSD1306_WHITE);
    display.fillRect(obs1.x - obs1.w/2 + 2, obs1.h + obs1.slotSize, obs1.w - 4, SCREEN_HEIGHT - obs1.h - obs1.slotSize, SSD1306_WHITE);
    display.fillRect(obs1.x - obs1.w/2, obs1.h - 4, obs1.w, 4, SSD1306_WHITE);
    display.fillRect(obs1.x - obs1.w/2, obs1.h + obs1.slotSize, obs1.w, 4, SSD1306_WHITE);

    display.fillRect(obs2.x - obs2.w/2 + 2, 0, obs2.w - 4, obs2.h, SSD1306_WHITE);
    display.fillRect(obs2.x - obs2.w/2 + 2, obs2.h + obs2.slotSize, obs2.w - 4, SCREEN_HEIGHT - obs2.h - obs1.slotSize, SSD1306_WHITE);
    display.fillRect(obs2.x - obs2.w/2, obs2.h - 4, obs2.w, 4, SSD1306_WHITE);
    display.fillRect(obs2.x - obs2.w/2, obs2.h + obs2.slotSize, obs2.w, 4, SSD1306_WHITE);
    
    //drawObstacle(obs1);
    //drawObstacle(obs2);
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);       
    display.setCursor(4, 4);
    display.print("score: ");
    display.print(score);
    display.display();
    
    if(joyStick.button())
      break;
  }

  menu(2);
  return;
}
