#include "Arduboy2Core.h"

#include "vars.h"

// For attack animation (controls 'reaping' flag)
template<typename T>
void animateOnce(T& structObj, uint8_t T::*cframe, uint8_t T::*framec,
                 int T::*counter, uint8_t T::*wait, uint24_t T::*sprite,
                 bool T::*talking) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe < structObj.*framec) {
      structObj.*cframe += 1;
    } else {
      structObj.*talking = false;  // Auto-reset flag
      structObj.*cframe = 0;       // Reset animation
    }
  }
  structObj.*counter += 1;
}
// For chests
template<typename T>
void animateThenFreeze(T& structObj, uint8_t T::*cframe, uint8_t T::*framec,
                       int T::*counter, uint8_t T::*wait, uint24_t T::*sprite,
                       bool T::*talking) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe < structObj.*framec) {
      structObj.*cframe += 1;
    } else {
      structObj.*talking = false;             // Auto-reset flag
      structObj.*cframe = structObj.*framec;  // freeze animation
      ardvoice.stopVoice();
      // currentSelection = 1;
      //eye.currentframe = 1;
    }
  }
  structObj.*counter += 1;
}
// For chick
template<typename T>
void animateThenFreeze2(T& structObj, uint8_t T::*cframe, uint8_t T::*framec,
                        int T::*counter, uint8_t T::*wait, uint24_t T::*sprite,
                        bool T::*talking) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe < structObj.*framec) {
      structObj.*cframe += 1;
    } else {
      structObj.*talking = false;             // Auto-reset flag
      structObj.*cframe = structObj.*framec;  // freeze animation
      ardvoice.stopVoice();
      currentSelection = 1;
      blockInput = false;
      //eye.currentframe = 1;
    }
  }
  structObj.*counter += 1;
}
template<typename T>
//This animaton function takes a sprite and runs through all its frames, then starts over at frame 0
void animateSprite(T& structObj, uint8_t T::*cframe, uint8_t T::*framec, int T::*counter, uint8_t T::*wait) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe < structObj.*framec) {
      structObj.*cframe += 1;
    } else {
      structObj.*cframe = 0;
    }
  }
  structObj.*counter += 1;
}


template<typename T>
//This animation function takes a sprite and runs trough all its frames in incremental order, then when reaching the final frame, in decremental order back to zero, ad infinitum
void animateFWB(T& structObj, uint8_t T::*cframe, uint8_t T::*framec, int T::*counter, uint8_t T::*wait, bool T::*inc) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe == structObj.*framec) {
      structObj.*inc = false;
    }
    if (structObj.*cframe == 0) {
      structObj.*inc = true;
    }
    if (structObj.*inc) {
      if (structObj.*cframe < structObj.*framec) {
        structObj.*cframe += 1;
      }
    } else {
      if (structObj.*cframe > 0) {
        structObj.*cframe -= 1;
      }
    }
  }
  structObj.*counter += 1;
}



void shuffleChests() {
  int weight = random(1, 4);
  if (weight == 1) {
    firstchest.hastreasure = true;

    secondchest.hastreasure = false;

    thirdchest.hastreasure = false;
  }
  if (weight == 2) {
    firstchest.hastreasure = false;

    secondchest.hastreasure = true;

    thirdchest.hastreasure = false;
  }
  if (weight == 3) {
    firstchest.hastreasure = false;

    secondchest.hastreasure = false;

    thirdchest.hastreasure = true;
  }
}
void handleChests() {
  switch (chestState) {
    case ChestState::Closed:
      if (arduboy.justPressed(A_BUTTON) && !picked) {
        picked = true;
        money -= bet;  // Deduct bet immediately
        chestProcessed = false;

        if (currentSelection == 1) {
          firstchest.talking = true;
          firstchest.currentframe = 0;
          firstchest.counter = 0;
        } else if (currentSelection == 2) {
          secondchest.talking = true;
          secondchest.currentframe = 0;
          secondchest.counter = 0;
        } else if (currentSelection == 3) {
          thirdchest.talking = true;
          thirdchest.currentframe = 0;
          thirdchest.counter = 0;
        }
        chestState = ChestState::Opening;
      }
      break;

    case ChestState::Opening:
      if (currentSelection == 1 && firstchest.talking) {
        animateThenFreeze(firstchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::talking);
        if (!firstchest.talking) {
          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }
      // Repeat for other chests...
      if (currentSelection == 2 && secondchest.talking) {
        animateThenFreeze(secondchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::talking);
        if (!secondchest.talking) {
          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }

      if (currentSelection == 3 && thirdchest.talking) {
        animateThenFreeze(thirdchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::talking);
        if (!thirdchest.talking) {
          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }
      break;

    case ChestState::Open:
      if (!chestProcessed) {
        chestProcessed = true;
        if (currentSelection == 1 && firstchest.hastreasure) {
          money += 2 * bet;  // Win: bet back + prize
        }

        // Add checks for other chests...
        if (currentSelection == 2 && secondchest.hastreasure) {
          money += 2 * bet;  // Win: bet back + prize
        }

        if (currentSelection == 3 && thirdchest.hastreasure) {
          money += 2 * bet;  // Win: bet back + prize
        }
      }

      if (resultTimer > 0) {
        resultTimer--;
      } else {
        if (money == 0) {
          gameOver = true;
        }
        chestState = ChestState::Resetting;
      }
      break;

    case ChestState::Resetting:
      // Reset all chests to closed state
      firstchest.talking = false;
      firstchest.currentframe = 0;
      secondchest.talking = false;
      secondchest.currentframe = 0;
      thirdchest.talking = false;
      thirdchest.currentframe = 0;

      shuffleChests();
      picked = false;
      chestState = ChestState::Closed;
      chick.talking = true;  // Make chick talk again
      break;
  }
}

void handleDrip(){


  
  if (drop.currentframe == drop.framecount){
    //physics
    //drop.position.y++;

    //drop.applyForce(force.gravity);
    drop.update();
    
  }
  if (drop.position.y >1504/*adjust for crude timing, higher the number, the longer the reset*/){
    drop.position.y = 0;
    drop.velocity.y = 0;
    startDripping = true;
  }
  if (startDripping) {
    
    drop.falling = true;
    drop.currentframe = 0;
    drop.counter = 0;

    startDripping = false;
  }
  if (drop.falling) {
    animateThenFreeze(drop, &Drops::currentframe, &Drops::framecount, &Drops::counter, &Drops::framewait, &Drops::sprite, &Drops::falling);
  }



  ///
}
void handleTalking() {

  if (startTalking) {
    blockInput = true;
    chick.talking = true;
    chick.currentframe = 0;
    chick.counter = 0;

    ardvoice.playVoice(voice, 0, 2790, 0.6);
    startTalking = false;
  }
  if (chick.talking) {
    animateThenFreeze2(chick, &Sprite::currentframe, &Sprite::framecount, &Sprite::counter, &Sprite::framewait, &Sprite::sprite, &Sprite::talking);
  }
}
void handleSelection() {
  eye.currentframe = currentSelection;
  if (currentSelection == 0) {
    //chick.talking = true;
  }
}

void playerInput() {
  if (!picked) {
    if (arduboy.justPressed(UP_BUTTON)) {
      pad.currentframe = 1;
      // Allow increasing bet only if it doesn't exceed money
      if (bet + 10 <= money) {
        bet += 10;
      }
    }
    if (arduboy.justPressed(DOWN_BUTTON)) {
      pad.currentframe = 2;
      if (bet > 10) {
        bet -= 10;
      }
    }
    if (arduboy.justPressed(LEFT_BUTTON)) {
      pad.currentframe = 3;
      if (currentSelection == 0) {
        currentSelection = 1;
      }
      if (currentSelection != 1) {
        --currentSelection;
      } else if (currentSelection == 1) {
        currentSelection = 3;
      }
    }
    if (arduboy.justReleased(LEFT_BUTTON) || arduboy.justReleased(RIGHT_BUTTON) || arduboy.justReleased(UP_BUTTON) || arduboy.justReleased(DOWN_BUTTON)) {
      pad.currentframe = 0;
    }

    if (arduboy.justPressed(RIGHT_BUTTON)) {
      pad.currentframe = 4;
      if (currentSelection != 3) {
        ++currentSelection;
      } else {
        currentSelection = 1;
      }
    }
  }
}
void update() {

  switch (screen) {
    case Screen::Splash:
      if (startcounter != 0) {
        startcounter--;
      } else {
        screen = Screen::Game;
        //handleTalking();
        startTalking = true;
        shuffleChests();
      }
      break;
    case Screen::Game:
      if (!blockInput) {
        playerInput();
      }
      handleSelection();
      handleChests();
      //handleMoney();
      handleTalking();
      handleDrip();
      if (gameOver) {
        screen = Screen::Lose;
      }

      break;

    case Screen::Lose:

      break;
  }
}


void render() {
  uint16_t currentPlane = arduboy.currentPlane();

  switch (screen) {

    case Screen::Splash:
      SpritesU::drawPlusMaskFX(0, 0, punklogooutlined, FRAME(0));
      break;


    case Screen::Game:

      if (currentPlane <= 2) {
#ifndef OLED_SH1106
        //arduboy.fillScreen(WHITE);
#endif
      }
      //SpritesU::fillRect_i8(Bbutton.x + 18, Bbutton.y, 100, 8, arduboy.color(BLACK));

      // Progress bar fill
      // uint16_t barWidth = (nanacounter > 500 ? 500 : nanacounter) / 5;  // 500 = 100px
      //SpritesU::fillRect_i8(Bbutton.x + 18, Bbutton.y, barWidth, 8, arduboy.color(DARK_GRAY));
      SpritesU::drawPlusMaskFX(0, 0, background, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, background, FRAME(1));

      SpritesU::drawPlusMaskFX(chick.x, chick.y, chick.sprite, FRAME(chick.currentframe));
      SpritesU::drawPlusMaskFX(128 - 16, 60 - 16, stuff, FRAME(1));  //skull
      SpritesU::drawPlusMaskFX(128 - 36, 60 - 16, stuff, FRAME(0));  //mushroom
      SpritesU::drawPlusMaskFX(drop.position.x, drop.position.y, drop.sprite, FRAME(drop.currentframe));



      SpritesU::drawPlusMaskFX(pad.x, pad.y, pad.sprite, FRAME(pad.currentframe));
      //SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));
      SpritesU::drawPlusMaskFX(firstchest.x, firstchest.y, firstchest.sprite, FRAME(firstchest.currentframe));
      SpritesU::drawPlusMaskFX(secondchest.x, secondchest.y, secondchest.sprite, FRAME(secondchest.currentframe));
      SpritesU::drawPlusMaskFX(thirdchest.x, thirdchest.y, thirdchest.sprite, FRAME(thirdchest.currentframe));
      SpritesU::drawPlusMaskFX(eye.x, eye.y, eye.sprite, FRAME(eye.currentframe));


      // Draw peel counter
      arduboy.setCursor(0, 0);
      arduboy.print("Bet $");
      arduboy.print(bet);
      arduboy.setCursor(100, 56);
      arduboy.print("$");
      arduboy.print(money);

      arduboy.setCursor(100, 0);
      arduboy.print("D");
      arduboy.print(openDelay);

      // Draw completion text

      break;

    case Screen::Lose:
      SpritesU::drawPlusMaskFX(0, 0, gameoverscreen, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, gameoverscreen, FRAME(1));

      break;
  }
}