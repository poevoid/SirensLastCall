#pragma once
//#include "Vector.h"
#include "voice.h"
#define FRAME(x) x * 3 + arduboy.currentPlane()
#define MAX_FPS 10
#define SPLASH_TIME 75
#define TOP_SPEED 3
#define UPPER_LID_CLOSED 14
#define UPPER_LID_OPEN   4
#define LOWER_LID_CLOSED 24
#define LOWER_LID_OPEN   28


// Simplified Drops structure
struct Drops {
  uint8_t x;
  float y;
  float vy;
  uint24_t sprite;
  uint8_t framecount;
  uint8_t currentframe;
  uint8_t framewait;
  int counter;
  bool moving;
  
  void update() { vy += 0.1; y += vy; }
};

struct Sprite {
  int x, y;
  uint24_t sprite;
  uint8_t framecount;
  uint8_t currentframe;
  uint8_t framewait;
  int counter;
  bool inc;
  bool moving;
};

struct Chest {
  int x, y;
  uint24_t sprite;
  uint8_t framecount;
  uint8_t currentframe;
  uint8_t framewait;
  int counter;
  bool hastreasure;
  bool moving;
};

enum class MenuSelect : uint8_t {
  Play,
  Help,
  Credits
};

enum class ModeSelect : uint8_t {
  Easy, 
  Normal,
  Hard
};

enum class Screen : uint8_t {
  Splash,
  Title,
  Mode,
  Game,
  Escape,
  Win,
  Lose,
  Credits,
  Help
};

enum class ChestState : uint8_t {
  Closed,
  Opening,
  Open,
  Resetting
};

uint16_t currentPlane;

uint8_t startcounter = SPLASH_TIME;

uint8_t helpIndex = 0;

uint8_t currentSelection = 0;
int resultTimer = 60;  // Frames to show open chest
int openDelay = 100;
bool startPrize =false;
bool blockInput = true;
bool picked = false;
bool chestProcessed = false;

long money = 100;
long bet = 10;

        uint8_t secondphase = 60;
        uint8_t thirdphase = 60;
uint8_t lives = 1;
int amountToWin=150;
bool tryAgain = true;
bool betlives = false;
bool gameOver = false;

int blinkcounter =0;
bool startTalking = false;
bool startDripping = true;
bool startBlinking = true;

uint8_t escaper = 25;
bool escaperright = true;
bool escaperleft= false;

Drops drop = {74, 0, 0.02, drip, 7, 0, 5, 0, false};

Chest firstchest = { 24, 42, trej, 5, 0, 4, 0, false, false };
Chest secondchest = { 44, 42, trej, 5, 0, 4, 0, true, false };
Chest thirdchest = { 64, 42, trej, 5, 0, 4, 0, false, false };

Sprite chick = { 80, 4, ladytalk, 8, 0, 4, 0, false, false };

Sprite frog = { 0, 13, froggy, 10, 0, 2, 0, false, false };
Sprite notes = { frog.x+16, frog.y-8, muzak, 5, 0, 9, 0, false, false };

Sprite pad = { 4, 39, dpad, 4, 0, 0, 0, false, false };
Sprite upad = { 29, 20, dpadupdown, 3, 0, 20, 0, false, false };
Sprite lrpad = { 29, 1, dpadlr, 3, 0, 20, 0, false, false };
Sprite Bbutton = {4, 43, bbutton, 3, 0, 19, 0, false, false};
Sprite Abutton = {128-16, 64-16, abutton, 3, 0, 19, 0, false, false};

Sprite helpscreens= {0,0,helpfirst,1, 0, 0, 0, false, false};
Sprite text = {10, 30, ctpaw, 7, 0, 4, 0, false, false};

Sprite menuCursor = { 87, 32, coin, 5, 0, 2, 0, false, false };
Sprite curse = { 87, 32, smoke, 13, 0, 1, 0, false, false };
Sprite Cursor = { 21, 29, cursor, 0, 0, 0, 0, false, false };
Sprite prize = { firstchest.x, firstchest.y, heart, 5, 0, 1, 0, false, false };
Sprite life = { 20, -3, heart, 5, 0, 9, 0, false, false };


Sprite eye = { 49, 16, buckbaird, 3, 0, 0, 0, false, false };
Sprite uplid = {eye.x, eye.y-2, upperlid, 0, 0, 1, 0};
Sprite lowlid = {eye.x, eye.y+8, lowerlid, 0, 0, 1, 0};
Sprite punk = {64, 0, creditpunk, 2, 0, 6, 0, true, true };

Screen screen = { Screen::Splash };
MenuSelect menu = {MenuSelect::Play};
ModeSelect currentMode = {ModeSelect::Easy};

ChestState chestState = ChestState::Closed;
