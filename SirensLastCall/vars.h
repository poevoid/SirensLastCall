#pragma once
#include "Vector.h"
#include "voice.h"
#define FRAME(x) x * 3 + arduboy.currentPlane()
#define MAX_FPS 10
#define SPLASH_TIME 75
#define TOP_SPEED 3
#define UPPER_LID_CLOSED 14
#define UPPER_LID_OPEN   4
#define LOWER_LID_CLOSED 24
#define LOWER_LID_OPEN   28
struct Force {
  Vector gravity;
  Vector wind;
  Vector friction;
  Vector drag;
};
Force force = {
  (0.1, 0),    //gravity
  (0.2, 0.5),  //wind
  (2, 2),      //friction
  (0, 0)       //drag
};
class Drops {
public:
  Vector position;
  Vector velocity;
  Vector acceleration;
  float mass;
  float weight;
  bool edgex;
  bool edgey;
  uint24_t sprite;
  uint8_t framecount;
  uint8_t currentframe;
  uint8_t framewait;
  int counter;
  bool inc;
  bool moving;

  void applyForce(Vector force) {
    acceleration = force / mass;  //F=MA || A=F/M Newtons second law
    //weight = mass / force.gravity
  }
  void draw() {
    arduboy.drawCircle(position.x, position.y, mass);
  }

  void update() {
    applyForce(force.gravity);
    if (acceleration.x > 0 || acceleration.y > 0) {  // Newtons first law,
      velocity += acceleration;                      //
    }
    position += velocity;
    acceleration *= 0;
  }
  void calcDrag() {
    float c = 0.1;
    float speed = velocity.mag();
    float dragmag = c * speed * speed;
    force.drag = velocity;
    force.drag *= -1;
    force.drag.setMag(dragmag);
  }
  void wrapAround() {
    if (position.x > WIDTH) {
      position.x = 0;
    } else if (position.x < 0) {
      position.x = WIDTH;
    }

    if (position.y > HEIGHT) {
      position.y = 0;
    } else if (position.y < 0) {
      position.y = HEIGHT;
    }
  }
};

 int startcounter = SPLASH_TIME;
uint16_t currentPlane;
uint8_t currentSelection = 0;
float voiceSpeed = 0.6;
int voiceSize = 2677;
long money = 100;
long bet = 10;
bool picked = false;
int openDelay = 100;
bool startTalking = false;
bool blockInput = true;
bool startDripping = true;




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
enum class Screen : uint8_t {
  Splash,
  Title,
  Game,
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

ChestState chestState = ChestState::Closed;
bool chestProcessed = false;
int resultTimer = 60;  // Frames to show open chest
bool gameOver = false;
bool startPrize =false;
bool tryAgain = true;
uint8_t lives = 1;
bool betlives = false;
bool startBlinking = true;
bool punkBlink = true;
int blinkcounter =0;
bool lowOpening = true;
bool upOpening = true;
uint8_t helpIndex = 0;
Chest firstchest = { 24, 42, trej, 5, 0, 4, 0, false, false };
Chest secondchest = { 44, 42, trej, 5, 0, 4, 0, true, false };
Chest thirdchest = { 64, 42, trej, 5, 0, 4, 0, false, false };
Sprite chick = { 128 - 48, 4, ladytalk, 8, 0, 4, 0, false, false };
Sprite pad = { 4, 39, dpad, 4, 0, 0, 0, false, false };
Sprite upad = { 29, 20, dpadupdown, 3, 0, 20, 0, false, false };
Sprite lrpad = { 29, 1, dpadlr, 3, 0, 20, 0, false, false };
//Sprite drop = { 74, 0, drip, 7, 0, 1, 0, false, false };
Drops drop;
Sprite menuCursor = { 87, 32, coin, 5, 0, 2, 0, false, false };
Sprite helpscreens= {0,0,helpfirst,1, 0, 0, 0, false, false};

Sprite prize = { firstchest.x, firstchest.y, heart, 5, 0, 1, 0, false, false };

Sprite life = { 20, -3, heart, 5, 0, 9, 0, false, false };

Sprite curse = { 87, 32, smoke, 13, 0, 1, 0, false, false };
Sprite Bbutton = {4, 43, bbutton, 3, 0, 19, 0, false, false};

Sprite Abutton = {128-16, 64-16, abutton, 3, 0, 19, 0, false, false};
Sprite Cursor = { 21, 29, cursor, 0, 0, 0, 0, false, false };
Sprite eye = { 49, 16, buckbaird, 3, 0, 0, 0, false, false };
Sprite uplid = {eye.x, eye.y-2, upperlid, 0, 0, 1, 0};
Sprite lowlid = {eye.x, eye.y+8, lowerlid, 0, 0, 1, 0};
Sprite punk = {64, 0, creditpunk, 2, 0, 6, 0, true, true };
Screen screen = { Screen::Splash };
MenuSelect menu = {MenuSelect::Play};