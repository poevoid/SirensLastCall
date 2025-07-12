#include "Arduboy2Core.h"

#include "vars.h"


// Combined animation templates
template<typename T>
void animate(T& s, bool loop = true, bool freeze = false, bool effects = false) {
  if (s.counter++ % FRAME(s.framewait)) return;

  if (s.currentframe < s.framecount) {
    s.currentframe++;
  } else {
    s.moving = false;
    if (loop) s.currentframe = 0;
    if (freeze) s.currentframe = s.framecount;
    if (effects) {
      ardvoice.stopVoice();
      currentSelection = 1;
    }
  }
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

void handleWinThreshold() {
  if (money >= amountToWin) {
    startTalking = true;
    //ardvoice.stopVoice();
    shuffleChests();
    resultTimer = 60;
    screen = Screen::Escape;
  }
}

void handleChests() {
  if (!blockInput) {
    // Simplified handleChests with array-based logic
    switch (chestState) {
      case ChestState::Closed:
        if (arduboy.justPressed(A_BUTTON) && !picked && money >= bet) {
          picked = true;
          money -= bet;
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
          animate(firstchest, false, true, false);
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
            resultTimer = 30;
          }
        }
        if (currentSelection == 2 && secondchest.moving) {
          animate(secondchest, false, true, false);
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
            resultTimer = 30;
          }
        }
        if (currentSelection == 3 && thirdchest.moving) {
          animate(thirdchest, false, true, false);
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
            resultTimer = 30;
          }
        }
        break;


      case ChestState::Open:
        if (!chestProcessed) {
          chestProcessed = true;
          startPrize = true;
          if (!betlives && currentSelection == 1 && firstchest.hastreasure) {
            money += (2 * bet);  // Win: bet back + prize
          } else if (betlives && currentSelection == 1 && firstchest.hastreasure) {
            money += 50;  // Win: bet back + prize
            lives = 1;
          }
          if (!betlives && currentSelection == 2 && secondchest.hastreasure) {
            money += (2 * bet);  // Win: bet back + prize
          } else if (betlives && currentSelection == 2 && secondchest.hastreasure) {
            money += 50;  // Win: bet back + prize
            lives = 1;
          }
          if (!betlives && currentSelection == 3 && thirdchest.hastreasure) {
            money += (2 * bet);  // Win: bet back + prize
          } else if (betlives && currentSelection == 3 && thirdchest.hastreasure) {
            money += 50;  // Win: bet back + prize
            lives = 1;
            //resultTimer = 120;
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
            animate(prize, false, false, false);
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


        break;
    }
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
  firstchest.currentframe = 0;

  secondchest.currentframe = 0;

  thirdchest.currentframe = 0;

  currentSelection = 0;
  gameOver = false;
  betlives = false;
  lives = 1;
  text.sprite = ctpaw;
  text.framewait = chick.framewait;
  text.currentframe = 0;
  frog.x = 0;
  pad.x = 4;
  chick.x = 80;
  firstchest.y = 42;
  secondchest.y = 42;
  thirdchest.y = 42;
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
    animate(punk, true, false, false);
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
  if (drop.y > 1504 /*adjust for crude timing, higher the number, the longer the reset*/) {
    drop.y = 0;
    drop.vy = 0;
    startDripping = true;
  }
  if (startDripping) {

    drop.moving = true;
    drop.currentframe = 0;
    drop.counter = 0;

    startDripping = false;
  }
  if (drop.moving) {
    animate(drop, false, true, false);
  }
}

void handleEscapeBar() {
  if (escaperleft) {
    escaper -= 2;
  }
  if (escaperright) {
    escaper += 2;
  }
  if (escaperright && escaper >= 103) {
    escaperright = false;
    escaperleft = true;
  }
  if (escaperleft && escaper <= 25) {
    escaperright = true;
    escaperleft = false;
  }
  if (arduboy.justPressed(A_BUTTON)) {
    if (escaper > 55 && escaper < 70) {
      text.sprite = endtext;
      text.framewait = Bbutton.framewait;
      resultTimer = 200;
      ardvoice.stopVoice();
      screen = Screen::Win;
    } else {
      screen = Screen::Lose;
    }
  }
}

void handleEscapeTalking() {
  if (startTalking) {
    blockInput = true;
    chick.moving = true;
    chick.currentframe = 0;
    chick.counter = 0;
    startTalking = false;
  }
  if (chick.moving) {
    blockInput = true;
    animate(chick, false, true, false);
  } else {
    chick.currentframe = 0;
    blockInput = false;
  }
}
void handleTalking() {

  if (startTalking && money < amountToWin) {
    blockInput = true;
    chick.moving = true;
    chick.currentframe = 0;
    chick.counter = 0;
    text.moving = true;
    text.currentframe = 0;
    text.counter = 0;
    ardvoice.playVoice(voice, 0, 2677, 0.6);
    startTalking = false;
  }
  if (chick.moving) {
    blockInput = true;
    animate(chick, false, true, true);

    animate(text, false, true, false);
  } else {
    text.currentframe = 0;
    blockInput = false;
  }
}
void handleEye() {
  eye.currentframe = currentSelection;
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
      if (!ardvoice.isVoicePlaying()) {
        ardvoice.playVoice(audio_q1);
      }
      animate(menuCursor, true, false, false);

      //T& structObj, uint8_t T::*cframe, uint8_t T::*framec, int T::*counter, uint8_t T::*wait
      switch (menu) {
        case MenuSelect::Play:
          menuCursor.x = 87;
          menuCursor.y = 31;
          if (arduboy.justPressed(DOWN_BUTTON)) {
            menu = MenuSelect::Help;
          }
          if (arduboy.justPressed(A_BUTTON)) {
            screen = Screen::Mode;
            //startTalking = true;
            //shuffleChests();
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


    case Screen::Help:

      switch (helpIndex) {
        case 0:
          animate(upad, true, false, false);
          animate(lrpad, true, false, false);
          animate(menuCursor, true, false, false);
          //animate(Abutton, true, false, false);
          animate(Bbutton, true, false, false);
          if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(RIGHT_BUTTON)) {
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
          //animate(Abutton, true, false, false);
          animate(Bbutton, true, false, false);
          if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(RIGHT_BUTTON)) {
            helpscreens.sprite = helpthird;
            helpIndex = 2;
          }
          if (arduboy.justPressed(B_BUTTON) || arduboy.justPressed(LEFT_BUTTON)) {
            helpscreens.sprite = helpfirst;
            helpIndex = 0;
          }
          break;

        case 2:

          animate(menuCursor, true, false, false);
          animate(life, true, false, false);
          //animate(Abutton, true, false, false);
          animate(Bbutton, true, false, false);
          if (arduboy.justPressed(A_BUTTON)) {
            helpscreens.sprite = helpfirst;
            helpIndex = 0;
            // pad.x = 4;
            // pad.y = 39;
            screen = Screen::Title;
          }
          if (arduboy.justPressed(B_BUTTON) || arduboy.justPressed(LEFT_BUTTON)) {
            helpscreens.sprite = helpsecond;
            helpIndex = 1;
          }

          break;
      }
      break;

    case Screen::Credits:
      timeBlinking();
      blinky();
      animate(Abutton, true, false, false);
      if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        screen = Screen::Title;
      }
      break;


    case Screen::Mode:
      if (arduboy.justPressed(A_BUTTON)) {
        startTalking = true;
        shuffleChests();
        screen = Screen::Game;
        Cursor.x = 21;
        Cursor.y = 29;
      }
      if (arduboy.justPressed(B_BUTTON)) {

        screen = Screen::Title;
      }

      switch (currentMode) {

        case ModeSelect::Easy:
          Cursor.x = 10;
          Cursor.y = 0;
          amountToWin = 150;
          if (arduboy.justPressed(DOWN_BUTTON)) {
            currentMode = ModeSelect::Normal;
          }
          break;

        case ModeSelect::Normal:
          Cursor.x = 10;
          Cursor.y = 24;
          amountToWin = 300;
          if (arduboy.justPressed(UP_BUTTON)) {
            currentMode = ModeSelect::Easy;
          }
          if (arduboy.justPressed(DOWN_BUTTON)) {
            currentMode = ModeSelect::Hard;
          }
          break;


        case ModeSelect::Hard:
          Cursor.x = 10;
          Cursor.y = 44;
          amountToWin = 1000;
          if (arduboy.justPressed(UP_BUTTON)) {
            currentMode = ModeSelect::Normal;
          }
          break;
      }
      break;

    case Screen::Game:
      if (!blockInput) {
        playerInput();
        if (!ardvoice.isVoicePlaying()) {
          ardvoice.playVoice(audio_q1);
        }
      }

      animate(frog, true, false, false);
      animate(notes, true, false, false);
      animate(menuCursor, true, false, false);
      handleEye();
      handleWinThreshold();
      handleChests();
      timeBlinking();
      handleBlinking();
      handleTalking();
      handleDrip();
      if (betlives) {
        animate(life, true, false, false);
      }
      if (gameOver) {
        ardvoice.stopVoice();
        screen = Screen::Lose;
      }

      break;

    case Screen::Lose:
      if (!ardvoice.isVoicePlaying()) {
        ardvoice.playVoice(doop);
      }
      if (tryAgain) {
        Cursor.x = 25;
        Cursor.y = 26;
        if (arduboy.justPressed(DOWN_BUTTON)) {
          tryAgain = false;
        }
        if (arduboy.justPressed(A_BUTTON)) {
          resetGame();

          ardvoice.stopVoice();
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

          ardvoice.stopVoice();
          screen = Screen::Title;
        }
      }



      break;
    case Screen::Win:
      ardvoice.stopVoice();
      if (resultTimer > 0) {
              resultTimer--;
      } else {
        if (secondphase > 0) {
          
          secondphase--;
        } else {
          
          if (thirdphase > 0) {
            
            thirdphase--;
          } else {
            resetGame();
            screen = Screen::Credits;
          }
        }
      }

      animate(text, false, true, false);

      break;

    case Screen::Escape:
      if (resultTimer > 0) {


        if (firstchest.y < HEIGHT) {
          firstchest.y++;
        }
        if (secondchest.y < HEIGHT) {
          secondchest.y++;
        }
        if (thirdchest.y < HEIGHT) {
          thirdchest.y++;
        }
        if (pad.x > -16) {

          pad.x--;
        }

        if (frog.x > -16) {

          frog.x--;
        }
        if (chick.x > eye.x - 8) {
          --chick.x;
        }
        handleEscapeTalking();
        resultTimer--;
      } else {
        if (!blockInput) {

          handleEscapeBar();
          //chick.currentframe=0;
        }
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

    case Screen::Mode:

      SpritesU::drawPlusMaskFX(32, 0, selectscreen, FRAME(0));

      SpritesU::drawPlusMaskFX(Cursor.x, Cursor.y, Cursor.sprite, FRAME(Cursor.currentframe));

      break;


    case Screen::Game:

      SpritesU::drawPlusMaskFX(eye.x, eye.y, eye.sprite, FRAME(eye.currentframe));
      SpritesU::drawPlusMaskFX(lowlid.x, lowlid.y, lowlid.sprite, FRAME(lowlid.currentframe));


      SpritesU::drawPlusMaskFX(uplid.x, uplid.y, uplid.sprite, FRAME(uplid.currentframe));


      SpritesU::drawPlusMaskFX(0, 0, background, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, background, FRAME(1));

      SpritesU::drawPlusMaskFX(chick.x, chick.y, chick.sprite, FRAME(chick.currentframe));
      SpritesU::drawPlusMaskFX(128 - 16, 60 - 16, stuff, FRAME(1));  //skull
      SpritesU::drawPlusMaskFX(128 - 36, 60 - 16, stuff, FRAME(0));  //mushroom
      SpritesU::drawPlusMaskFX(drop.x, drop.y, drop.sprite, FRAME(drop.currentframe));



      SpritesU::drawPlusMaskFX(frog.x, frog.y, frog.sprite, FRAME(frog.currentframe));

      SpritesU::drawPlusMaskFX(notes.x, notes.y, notes.sprite, FRAME(notes.currentframe));
      SpritesU::drawPlusMaskFX(pad.x, pad.y, pad.sprite, FRAME(pad.currentframe));
      //SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));
      SpritesU::drawPlusMaskFX(firstchest.x, firstchest.y, firstchest.sprite, FRAME(firstchest.currentframe));
      SpritesU::drawPlusMaskFX(secondchest.x, secondchest.y, secondchest.sprite, FRAME(secondchest.currentframe));
      SpritesU::drawPlusMaskFX(thirdchest.x, thirdchest.y, thirdchest.sprite, FRAME(thirdchest.currentframe));
      drawPrizes();
      if (text.moving) {

        SpritesU::drawPlusMaskFX(text.x, text.y, text.sprite, FRAME(text.currentframe));
      }

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

    case Screen::Escape:


      // SpritesU::drawPlusMaskFX(eye.x, eye.y, eye.sprite, FRAME(eye.currentframe));
      //SpritesU::drawPlusMaskFX(lowlid.x, lowlid.y, lowlid.sprite, FRAME(lowlid.currentframe));


      //SpritesU::drawPlusMaskFX(uplid.x, uplid.y, uplid.sprite, FRAME(uplid.currentframe));


      SpritesU::drawPlusMaskFX(0, 0, background, FRAME(0));
      SpritesU::drawPlusMaskFX(64, 0, background, FRAME(1));

      SpritesU::drawPlusMaskFX(128 - 16, 60 - 16, stuff, FRAME(1));  //skull
      SpritesU::drawPlusMaskFX(128 - 36, 60 - 16, stuff, FRAME(0));  //mushroom
                                                                     // SpritesU::drawPlusMaskFX(drop.position.x, drop.position.y, drop.sprite, FRAME(drop.currentframe));



      SpritesU::drawPlusMaskFX(chick.x, chick.y, chick.sprite, FRAME(chick.currentframe));

      //SpritesU::drawPlusMaskFX(notes.x, notes.y, notes.sprite, FRAME(notes.currentframe));
      //SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));

      if (firstchest.y < HEIGHT) {
        SpritesU::drawPlusMaskFX(firstchest.x, firstchest.y, firstchest.sprite, FRAME(firstchest.currentframe));
      }
      if (secondchest.y < HEIGHT) {
        SpritesU::drawPlusMaskFX(secondchest.x, secondchest.y, secondchest.sprite, FRAME(secondchest.currentframe));
      }
      if (thirdchest.y < HEIGHT) {
        SpritesU::drawPlusMaskFX(thirdchest.x, thirdchest.y, thirdchest.sprite, FRAME(thirdchest.currentframe));
      }
      if (pad.x > -16) {
        SpritesU::drawPlusMaskFX(pad.x, pad.y, pad.sprite, FRAME(pad.currentframe));
      }

      if (frog.x > -16) {
        SpritesU::drawPlusMaskFX(frog.x, frog.y, frog.sprite, FRAME(frog.currentframe));
      }
      if (!blockInput) {
        SpritesU::fillRect_i8(25, 56, 78, 8, arduboy.color(BLACK));

        SpritesU::fillRect_i8(55, 56, 15, 8, arduboy.color(DARK_GRAY));
        SpritesU::fillRect_i8(escaper, 56, 1, 8, arduboy.color(WHITE));

      } if (resultTimer>0){
        SpritesU::drawPlusMaskFX(text.x, 40, easyq, FRAME(0));
      }


      break;
    case Screen::Win:
      if (resultTimer > 0) {
        SpritesU::drawPlusMaskFX(0, 0, tunnel, FRAME(0));
        SpritesU::drawPlusMaskFX(64, 0, tunnel, FRAME(1));
        SpritesU::drawPlusMaskFX(text.x, text.y, text.sprite, FRAME(text.currentframe));
        //resultTimer--;
      } else {
        if (secondphase > 0) {
          SpritesU::drawPlusMaskFX(0, 0, tunneltwo, FRAME(0));
          SpritesU::drawPlusMaskFX(64, 0, tunneltwo, FRAME(1));
          //secondphase--;
        } else {
          
          if (thirdphase > 0) {
            SpritesU::drawPlusMaskFX(0, 0, tunnelthree, FRAME(0));
            SpritesU::drawPlusMaskFX(64, 0, tunnelthree, FRAME(1));
            //thirdphase--;
          } else {
           // resetGame();
            //screen = Screen::Credits;
          }
        }
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
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Bbutton.currentframe));
          SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));

          break;

        case 1:
          SpritesU::drawPlusMaskFX(0, helpscreens.y, helpscreens.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(64, helpscreens.y, helpscreens.sprite, FRAME(1));
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Bbutton.currentframe));
          SpritesU::drawPlusMaskFX(Bbutton.x, Bbutton.y, Bbutton.sprite, FRAME(Bbutton.currentframe));


          break;

        case 2:
          SpritesU::drawPlusMaskFX(0, helpscreens.y, helpscreens.sprite, FRAME(0));
          SpritesU::drawPlusMaskFX(64, helpscreens.y, helpscreens.sprite, FRAME(1));
          SpritesU::drawPlusMaskFX(109, 43, Abutton.sprite, FRAME(Bbutton.currentframe));
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