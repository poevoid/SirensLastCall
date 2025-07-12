



#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION
//#define OLED_SH1106  //for arduboy mini, set ABG_REFRESH_HZ TO 95
#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_FX
#define SPRITESU_RECT
//#include <EEPROM.h>


//#include <ArduboyFX.h>

#include "src/ArdVoice.h"
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
  arduboy.initRandomSeed();
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