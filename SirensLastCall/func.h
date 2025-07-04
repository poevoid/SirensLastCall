#include "Arduboy2Core.h"

#include "vars.h"

// Standard animation - loops continuously
template<typename T>
void animateSprite(T& s) {
  if (s.counter % FRAME(s.framewait) == 0) {
    s.currentframe = (s.currentframe < s.framecount) ? s.currentframe + 1 : 0;
  }
  s.counter++;
}

// Animation that runs once then resets
template<typename T>
void animateOnce(T& s) {
  if (s.moving) {
    if (s.counter % FRAME(s.framewait) == 0) {
      if (s.currentframe < s.framecount) {
        s.currentframe++;
      } else {
        s.moving = false;
        s.currentframe = 0;
      }
    }
    s.counter++;
  }
}

template<typename T>
void animateThenFreeze(T& structObj, uint8_t T::*cframe, uint8_t T::*framec,
                       int T::*counter, uint8_t T::*wait, uint24_t T::*sprite,
                       bool T::*moving) {
  if (structObj.*counter % (FRAME(structObj.*wait)) == 0) {
    if (structObj.*cframe < structObj.*framec) {
      structObj.*cframe += 1;
    } else {
      structObj.*moving = false;              // Auto-reset flag
      structObj.*cframe = structObj.*framec;  // freeze animation
      //ardvoice.stopVoice();
      // currentSelection = 1;
      //eye.currentframe = 1;
    }
  }
  structObj.*counter += 1;
}

// Specialized animation with side effects (chick-specific)
template<typename T>
void animateThenFreeze2(T& s) {
  if (s.counter % FRAME(s.framewait) == 0) {
    if (s.currentframe < s.framecount) {
      s.currentframe++;
    } else {
      s.moving = false;
      s.currentframe = s.framecount;
      ardvoice.stopVoice();
      currentSelection = 1;
      blockInput = false;
    }
  }
  s.counter++;
}
// Specialized animation with side effects (chick-specific)
template<typename T>
void animateThenFreeze3(T& s) {
  if (s.counter % FRAME(s.framewait) == 0) {
    if (s.currentframe < s.framecount) {
      s.currentframe++;
    } else {
      s.moving = false;
      s.currentframe = s.framecount;
      //ardvoice.stopVoice();
      //currentSelection = 1;
      //blockInput = false;
    }
  }
  s.counter++;
}
// Forward-backward animation (ping-pong)
template<typename T>
void animateFWB(T& s) {
  if (s.counter % FRAME(s.framewait) == 0) {
    if (s.currentframe == s.framecount) s.inc = false;
    if (s.currentframe == 0) s.inc = true;

    if (s.inc && s.currentframe < s.framecount) {
      s.currentframe++;
    } else if (!s.inc && s.currentframe > 0) {
      s.currentframe--;
    }
  }
  s.counter++;
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
      if (betlives) {
      }


      if (arduboy.justPressed(A_BUTTON) && !picked && money >= 10) {
        picked = true;
        money -= bet;  // Deduct bet immediately
        chestProcessed = false;

        if (currentSelection == 1) {
          firstchest.moving = true;
          firstchest.currentframe = 0;
          firstchest.counter = 0;
        } else if (currentSelection == 2) {
          secondchest.moving = true;
          secondchest.currentframe = 0;
          secondchest.counter = 0;
        } else if (currentSelection == 3) {
          thirdchest.moving = true;
          thirdchest.currentframe = 0;
          thirdchest.counter = 0;
        }
        chestState = ChestState::Opening;
      }
      if (arduboy.justPressed(A_BUTTON) && !picked && betlives) {
        picked = true;
        // money -= bet;  // Deduct bet immediately
        lives -= 1;
        chestProcessed = false;

        if (currentSelection == 1) {
          firstchest.moving = true;
          firstchest.currentframe = 0;
          firstchest.counter = 0;
        } else if (currentSelection == 2) {
          secondchest.moving = true;
          secondchest.currentframe = 0;
          secondchest.counter = 0;
        } else if (currentSelection == 3) {
          thirdchest.moving = true;
          thirdchest.currentframe = 0;
          thirdchest.counter = 0;
        }
        chestState = ChestState::Opening;
      }
      break;

    case ChestState::Opening:
      if (currentSelection == 1 && firstchest.moving) {
        animateThenFreeze(firstchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::moving);
        if (!firstchest.moving) {
          if (firstchest.hastreasure) {
            prize.x = firstchest.x + 4;
            prize.y = firstchest.y;
            prize.sprite = coin;
            prize.framecount = 5;
            prize.framewait = 2;
          } else {
            prize.x = firstchest.x - 8;
            prize.y = firstchest.y - 24;
            prize.sprite = smoke;
            prize.framecount = 13;
            prize.framewait = 1;
          }
          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }
      // Repeat for other chests...
      if (currentSelection == 2 && secondchest.moving) {
        animateThenFreeze(secondchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::moving);
        if (!secondchest.moving) {
          if (secondchest.hastreasure) {
            prize.x = secondchest.x + 4;
            prize.y = secondchest.y;
            prize.sprite = coin;
            prize.framecount = 5;
            prize.framewait = 2;
          } else {
            prize.x = secondchest.x - 8;
            prize.y = secondchest.y - 24;
            prize.sprite = smoke;
            prize.framecount = 13;
            prize.framewait = 1;
          }
          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }

      if (currentSelection == 3 && thirdchest.moving) {
        animateThenFreeze(thirdchest, &Chest::currentframe, &Chest::framecount,
                          &Chest::counter, &Chest::framewait, &Chest::sprite,
                          &Chest::moving);
        if (!thirdchest.moving) {
          if (thirdchest.hastreasure) {
            prize.x = thirdchest.x + 4;
            prize.y = thirdchest.y;
            prize.sprite = coin;
            prize.framecount = 5;
            prize.framewait = 2;
          } else {
            prize.x = thirdchest.x - 8;
            prize.y = thirdchest.y - 24;
            prize.sprite = smoke;
            prize.framecount = 13;
            prize.framewait = 1;
          }


          chestState = ChestState::Open;
          resultTimer = 60;  // Reset timer
        }
      }
      break;

    case ChestState::Open:
      if (!chestProcessed) {
        chestProcessed = true;
        startPrize = true;
        //bet = 10;

        if (!betlives && currentSelection == 1 && firstchest.hastreasure) {
          money += (2 * bet);  // Win: bet back + prize

          //resultTimer = 120;

          //animateSprite(life);
        } else if (!betlives && currentSelection == 1 && !firstchest.hastreasure) {

          //resultTimer = 120;
          // animateOnce(curse);
        } else if (betlives && currentSelection == 1 && firstchest.hastreasure) {
          money += 50;  // Win: bet back + prize
                        //resultTimer = 120;
          lives = 1;
          //animateSprite(life);
        } else if (betlives && currentSelection == 1 && !firstchest.hastreasure) {
          //resultTimer = 120;
          // animateOnce(curse);
        }


        // Add checks for other chests...
        if (!betlives && currentSelection == 2 && secondchest.hastreasure) {
          money += (2 * bet);  // Win: bet back + prize
          //resultTimer = 120;
        } else if (!betlives && currentSelection == 2 && !secondchest.hastreasure) {
          //resultTimer = 120;
          //animateOnce(curse);
        } else if (betlives && currentSelection == 2 && secondchest.hastreasure) {
          money += 50;  // Win: bet back + prize
          lives = 1;

          //resultTimer = 120;
        } else if (betlives && currentSelection == 2 && !secondchest.hastreasure) {
          //resultTimer = 120;
          //animateOnce(curse);
        }

        if (!betlives && currentSelection == 3 && thirdchest.hastreasure) {
          money += (2 * bet);  // Win: bet back + prize
          //resultTimer = 120;
        } else if (!betlives && currentSelection == 3 && !thirdchest.hastreasure) {
          //resultTimer = 120;
          //animateOnce(curse);
        }
        if (betlives && currentSelection == 3 && thirdchest.hastreasure) {
          money += 50;  // Win: bet back + prize
          lives = 1;
          //resultTimer = 120;
        } else if (betlives && currentSelection == 3 && !thirdchest.hastreasure) {
          //resultTimer = 120;
          //animateOnce(curse);
        }
        if (bet > money) {
          bet = 10;
        }
      }

      if (resultTimer > 0) {
        if (startPrize) {

          prize.moving = true;
          prize.currentframe = 0;
          prize.counter = 0;
          startPrize = false;
        }
        if (prize.moving) {
          animateThenFreeze3(prize);
        }

        resultTimer--;
      } else {


        if (money == 0 && lives == 0) {
          gameOver = true;
        } else if (money == 0) {
          betlives = true;
        } else {
          betlives = false;
        }
        chestState = ChestState::Resetting;
      }
      break;

    case ChestState::Resetting:
      // Reset all chests to closed state
      firstchest.moving = false;
      firstchest.currentframe = 0;
      secondchest.moving = false;
      secondchest.currentframe = 0;
      thirdchest.moving = false;
      thirdchest.currentframe = 0;

      shuffleChests();
      picked = false;
      chestState = ChestState::Closed;
      //chick.moving = true;  // Make chick talk again
      break;
  }
}
void drawPrizes() {
  if (chestState == ChestState::Open) {
    SpritesU::drawPlusMaskFX(prize.x, prize.y, prize.sprite, FRAME(prize.currentframe));
  }
}

void resetGame() {
  money = 100;
  startTalking = true;
  shuffleChests();
  currentSelection = 0;
  gameOver = false;
  betlives = false;
  lives = 1;
}
void timeBlinking() {
  if (blinkcounter > 0) {
    blinkcounter--;
  }
  if (blinkcounter == 0 && !startBlinking) {
    startBlinking = true;
  }
}
void blinky() {
  if (startBlinking) {

    punk.moving = true;  //pretend moving is "closing"

    blinkcounter = random(90, 450);

    startBlinking = false;
  }
  if (punk.moving) {
    animateOnce(punk);
  }
}
void handleBlinking() {
  if (startBlinking) {

    uplid.moving = true;   //pretend moving is "closing"
    lowlid.moving = true;  //pretend moving is "closing"

    startBlinking = false;

    blinkcounter = random(90, 250);
  }
  if (uplid.moving) {  //pretend moving is "closing"
    if (uplid.y < UPPER_LID_CLOSED) {
      uplid.y += 2;
    } else {
      uplid.moving = false;
    }
  } else {  //opening
    if (uplid.y > UPPER_LID_OPEN) {
      uplid.y -= 2;
    }
  }
  if (lowlid.moving) {  //pretend moving is "closing"
    if (lowlid.y > LOWER_LID_CLOSED) {
      lowlid.y--;
    } else {
      lowlid.moving = false;
    }
  } else {  //opening
    if (lowlid.y < LOWER_LID_OPEN) {
      lowlid.y++;
    }
  }
  /*if (startBlink){
  if (uplid.y < 20){
    uplid.y++;
  }
  if (lowlid.y > 20){
    lowlid.y--;
  } else {
    startBlink = false;
  }

  } else {
    if (uplid.y < eye.y-12){
      uplid.y--;
    } 
    if (lowlid.y < eye.y+12){
      lowlid.y++;
    }
  }*/
}
void handleDrip() {



  if (drop.currentframe >= drop.framecount - 1) {
    //physics
    //drop.position.y++;

    //drop.applyForce(force.gravity);
    drop.update();
  }
  if (drop.position.y > 1504 /*adjust for crude timing, higher the number, the longer the reset*/) {
    drop.position.y = 0;
    drop.velocity.y = 0;
    startDripping = true;
  }
  if (startDripping) {

    drop.moving = true;
    drop.currentframe = 0;
    drop.counter = 0;

    startDripping = false;
  }
  if (drop.moving) {
    animateThenFreeze(drop, &Drops::currentframe, &Drops::framecount,
                      &Drops::counter, &Drops::framewait, &Drops::sprite,
                      &Drops::moving);
  }



  ///
}
void handleTalking() {

  if (startTalking) {
    blockInput = true;
    chick.moving = true;
    chick.currentframe = 0;
    chick.counter = 0;

    ardvoice.playVoice(voice, 0, 2790, 0.6);
    startTalking = false;
  }
  if (chick.moving) {
    animateThenFreeze2(chick);
  }
}
void handleSelection() {
  eye.currentframe = currentSelection;
  if (currentSelection == 0) {
    //chick.moving = true;
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
        screen = Screen::Title;
        //handleTalking();
      }
      break;

    case Screen::Title:
      animateSprite(menuCursor);
      //T& structObj, uint8_t T::*cframe, uint8_t T::*framec, int T::*counter, uint8_t T::*wait
      switch (menu) {
        case MenuSelect::Play:
          menuCursor.x = 87;
          menuCursor.y = 31;
          if (arduboy.justPressed(DOWN_BUTTON)) {
            menu = MenuSelect::Help;
          }
          if (arduboy.justPressed(A_BUTTON)) {
            screen = Screen::Game;
            startTalking = true;
            shuffleChests();
          }
          break;

        case MenuSelect::Help:
          menuCursor.x = 90;
          menuCursor.y = 43;
          if (arduboy.justPressed(UP_BUTTON)) {
            menu = MenuSelect::Play;
          }
          if (arduboy.justPressed(DOWN_BUTTON)) {
            menu = MenuSelect::Credits;
          }
          if (arduboy.justPressed(A_BUTTON)) {
            screen = Screen::Help;
          }
          break;

        case MenuSelect::Credits:
          menuCursor.x = 76;
          menuCursor.y = 54;
          if (arduboy.justPressed(UP_BUTTON)) {
            menu = MenuSelect::Help;
          }
          if (arduboy.justPressed(A_BUTTON)) {
            screen = Screen::Credits;
          }
          break;
      }
      break;


    case Screen::Game:
      if (!blockInput) {
        playerInput();
      }

      animateSprite(menuCursor);
      handleSelection();
      handleChests();
      //handleMoney();
      timeBlinking();
      handleBlinking();
      handleTalking();
      handleDrip();
      if (betlives) {
        animateSprite(life);
      }
      if (gameOver) {
        screen = Screen::Lose;
      }

      break;

    case Screen::Lose:

      if (tryAgain) {
        Cursor.x = 25;
        Cursor.y = 26;
        if (arduboy.justPressed(DOWN_BUTTON)) {
          tryAgain = false;
        }
        if (arduboy.justPressed(A_BUTTON)) {
          resetGame();
          screen = Screen::Game;
        }
      } else {
        Cursor.x = 25;
        Cursor.y = 37;
        if (arduboy.justPressed(UP_BUTTON)) {
          tryAgain = true;
        }
        if (arduboy.justPressed(A_BUTTON)) {
          resetGame();
          screen = Screen::Title;
        }
      }



      break;
    case Screen::Help:

      switch (helpIndex) {
        case 0:
          animateSprite(upad);
          animateSprite(lrpad);
          animateSprite(menuCursor);
          animateSprite(Abutton);
          animateSprite(Bbutton);
          if (arduboy.justPressed(A_BUTTON)) {
            helpscreens.sprite = helpsecond;
            helpIndex = 1;
          }
          if (arduboy.justPressed(B_BUTTON)) {
            //helpscreens.sprite = helpfirst;
            //helpIndex = 0;
            // pad.x = 4;
            // pad.y = 39;
            screen = Screen::Title;
          }
          
          break;

        case 1:
          animateSprite(Abutton);
          animateSprite(Bbutton);
          if (arduboy.justPressed(A_BUTTON)) {
            helpscreens.sprite = helpthird;
            helpIndex = 2;
          }
          if (arduboy.justPressed(B_BUTTON)) {
            helpscreens.sprite = helpfirst;
            helpIndex = 0;
          }
          break;

        case 2:

          animateSprite(menuCursor);
          animateSprite(life);
          animateSprite(Abutton);
          animateSprite(Bbutton);
          if (arduboy.justPressed(A_BUTTON)) {
            helpscreens.sprite = helpfirst;
            helpIndex = 0;
            // pad.x = 4;
            // pad.y = 39;
            screen = Screen::Title;
          }
          if (arduboy.justPressed(B_BUTTON)) {
            helpscreens.sprite = helpsecond;
            helpIndex = 1;
          }

          break;
      }



      break;
    case Screen::Credits:
      timeBlinking();
      blinky();
      animateSprite(Abutton);
      if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        screen = Screen::Title;
      }
      break;
  }
}


void render() {
  uint16_t currentPlane = arduboy.currentPlane();

  switch (screen) {

    case Screen::Splash:
      SpritesU::drawPlusMaskFX(0, 0, punklogooutlined, FRAME(0));
      break;

    case Screen::Title:
      SpritesU::drawPlusMaskFX(0, 0, title, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, title, FRAME(1));
      SpritesU::drawPlusMaskFX(menuCursor.x, menuCursor.y, menuCursor.sprite, FRAME(menuCursor.currentframe));


      break;



    case Screen::Game:

      if (currentPlane <= 2) {
#ifndef OLED_SH1106
        //arduboy.fillScreen(WHITE);
#endif
      }
      SpritesU::drawPlusMaskFX(eye.x, eye.y, eye.sprite, FRAME(eye.currentframe));
      SpritesU::drawPlusMaskFX(lowlid.x, lowlid.y, lowlid.sprite, FRAME(lowlid.currentframe));


      SpritesU::drawPlusMaskFX(uplid.x, uplid.y, uplid.sprite, FRAME(uplid.currentframe));


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
      drawPrizes();


      arduboy.setCursor(0, 0);
      arduboy.print("Bet:");

      arduboy.setCursor(107, 56);
      //arduboy.print("$");
      arduboy.print(money);
      arduboy.fillRect(99, 56, 8, 8, BLACK);
      SpritesU::drawPlusMaskFX(99, 56, menuCursor.sprite, FRAME(menuCursor.currentframe));
      if (betlives) {
        arduboy.fillRect(life.x, life.y, 16, 11, BLACK);


        SpritesU::drawPlusMaskFX(life.x, life.y, life.sprite, FRAME(life.currentframe));

      } else {
        arduboy.setCursor(32, 0);
        arduboy.print(bet);

        arduboy.fillRect(24, 0, 8, 8, BLACK);
        SpritesU::drawPlusMaskFX(24, 0, menuCursor.sprite, FRAME(menuCursor.currentframe));
      }

      break;

    case Screen::Lose:
      SpritesU::drawPlusMaskFX(0, 0, gameoverscreen, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, gameoverscreen, FRAME(1));
      SpritesU::drawPlusMaskFX(Cursor.x, Cursor.y, Cursor.sprite, FRAME(Cursor.currentframe));

      break;

    case Screen::Credits:
      SpritesU::drawPlusMaskFX(0, 0, creditleft, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 32, creditpunkbottom, FRAME(0));
      SpritesU::drawPlusMaskFX(96, 32, creditpunkbottom, FRAME(1));
      SpritesU::drawPlusMaskFX(punk.x, punk.y, punk.sprite, FRAME(punk.currentframe));
      //SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));
      SpritesU::drawPlusMaskFX(108, 14, Abutton.sprite, FRAME(Abutton.currentframe));

      break;
    case Screen::Help:
      //if (currentPlane <= 2) {
      switch (helpIndex) {
        case 0:

          SpritesU::drawPlusMaskFX(0, helpscreens.y, helpscreens.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(64, helpscreens.y, helpscreens.sprite, FRAME(1));

          SpritesU::drawPlusMaskFX(93, 1, firstchest.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(upad.x, upad.y, upad.sprite, FRAME(upad.currentframe));
          SpritesU::drawPlusMaskFX(lrpad.x, lrpad.y, lrpad.sprite, FRAME(lrpad.currentframe));
          SpritesU::drawPlusMaskFX(92, 23, menuCursor.sprite, FRAME(menuCursor.currentframe));
          SpritesU::drawPlusMaskFX(38, 38, menuCursor.sprite, FRAME(menuCursor.currentframe));
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Abutton.currentframe));
          SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));

          break;

        case 1:
          SpritesU::drawPlusMaskFX(0, helpscreens.y, helpscreens.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(64, helpscreens.y, helpscreens.sprite, FRAME(1));
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Abutton.currentframe));
          SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));


          break;

        case 2:
          SpritesU::drawPlusMaskFX(0, helpscreens.y, helpscreens.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(64, helpscreens.y, helpscreens.sprite, FRAME(1));
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Abutton.currentframe));
          SpritesU::drawPlusMaskFX(128 - 14, 16, life.sprite, FRAME(life.currentframe));
          SpritesU::drawPlusMaskFX(72, 1, menuCursor.sprite, FRAME(menuCursor.currentframe));
          SpritesU::drawPlusMaskFX(94, 31, menuCursor.sprite, FRAME(menuCursor.currentframe));
          SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));


          break;
      }

      // arduboy.setCursor(0, 0);
      //arduboy.println(" Use   to pick a chest.\n \n\n Use   to change bet.\n");
      // }
      // SpritesU::drawPlusMaskFX(pad.x, pad.y, pad.sprite, FRAME(pad.currentframe));
      // SpritesU::drawPlusMaskFX(pad.x, pad.y+16, pad.sprite, FRAME(pad.currentframe));

      break;
  }
}