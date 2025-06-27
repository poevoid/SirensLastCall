



#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION
//define OLED_SH1106  //for arduboy mini, set ABG_REFRESH_HZ TO 95
#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_FX
#define SPRITESU_RECT
#include <ArduboyFX.h>
#include <ArdVoice.h>
//#include <EEPROM.h>
#include "src/ArduboyG.h"
#include "src/SpritesU.hpp"
#include "fxdata/fxdata.h"


ArduboyG_Config<ABG_Mode::L4_Triplane> arduboy;
ArdVoice ardvoice;
#include "func.h"

void setup() {

  arduboy.begin();

  arduboy.startGray();
  FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
  drop.mass =0.2;
  force.gravity.x = 0;
  force.gravity.y = 0.1;
  force.wind.x = 0;
  force.wind.y = 0;
  drop.sprite = drip;
  drop.position.x = 74;
  drop.position.y = 0;
  drop.framecount = 7;
  drop.currentframe = 0;
  drop.framewait = 1;
  drop.falling = false;
  drop.acceleration.x = 0;
  drop.acceleration.y = 0.01;
  drop.velocity.x = 0;
  drop.velocity.y = 0;
}

  void loop() {



    arduboy.waitForNextPlane();
    FX::disableOLED();
    if (arduboy.needsUpdate()) {
      arduboy.pollButtons();
      update();
    }
    render();
    FX::enableOLED();
  }