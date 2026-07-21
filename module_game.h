/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_game.h - Cyber Pong (kept from v2.0)
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"
#include "touch_driver.h"

extern TFT_eSPI tft;
extern PongState pong;
extern AppState currentState;

#define EXIT_BTN_X 250
#define EXIT_BTN_Y 218
#define EXIT_BTN_W 60
#define EXIT_BTN_H 18
#define PONG_FIELD_TOP 30
#define PONG_FIELD_LEFT 5
#define PONG_FIELD_RIGHT 315
#define PADDLE_WIDTH 50
#define PADDLE_HEIGHT 6
#define PADDLE_Y 200
#define PONG_FIELD_BOTTOM (PADDLE_Y + PADDLE_HEIGHT)
#define BALL_SIZE 6

inline void drawExitButton() {
  tft.drawRect(EXIT_BTN_X, EXIT_BTN_Y, EXIT_BTN_W, EXIT_BTN_H, COLOR_DANGER);
  tft.setTextColor(COLOR_DANGER);
  tft.setCursor(EXIT_BTN_X + 6, EXIT_BTN_Y + 5);
  tft.print("SAIR");
}

inline void drawPongScore() {
  tft.fillRect(160, 4, 155, 18, COLOR_BG);
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(160, 8);
  tft.printf("PONTOS:%d  ERROS:%d", pong.score, pong.misses);
}

inline void resetPongGame() {
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(10, 10); tft.print("CYBER PONG");
  tft.drawLine(0, 25, SCREEN_W, 25, COLOR_BORDER);
  tft.drawLine(0, 214, SCREEN_W, 214, COLOR_BORDER);
  tft.setTextColor(0x7BCF);
  tft.setCursor(10, 224);
  tft.print("Arraste o dedo para mover a raquete");
  drawExitButton();

  pong.paddleX = (SCREEN_W - PADDLE_WIDTH) / 2;
  pong.ballX = SCREEN_W / 2;
  pong.ballY = PONG_FIELD_TOP + 20;
  pong.ballVX = 2.2;
  pong.ballVY = 2.0;
  pong.score = 0;
  pong.misses = 0;
  pong.prevPaddleX = -1;
  pong.prevBallX = -1;
  pong.prevBallY = -1;
  drawPongScore();
}

inline void enterCyberPong() {
  currentState = STATE_GAME;
  resetPongGame();
}

inline void updatePongGame() {
  int sx = 0, sy = 0;
  if (readTouchScreen(sx, sy)) {
    if (sx >= EXIT_BTN_X && sx <= EXIT_BTN_X + EXIT_BTN_W &&
        sy >= EXIT_BTN_Y && sy <= EXIT_BTN_Y + EXIT_BTN_H) {
      currentState = STATE_MENU;
      // Caller will redraw
      return;
    }
    pong.paddleX = constrain(sx - PADDLE_WIDTH / 2, PONG_FIELD_LEFT, PONG_FIELD_RIGHT - PADDLE_WIDTH);
  }

  if (pong.prevPaddleX >= 0) {
    tft.fillRect(pong.prevPaddleX, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, COLOR_BG);
  }
  if (pong.prevBallX >= 0) {
    tft.fillRect((int)pong.prevBallX, (int)pong.prevBallY, BALL_SIZE, BALL_SIZE, COLOR_BG);
  }

  pong.ballX += pong.ballVX;
  pong.ballY += pong.ballVY;

  if (pong.ballX <= PONG_FIELD_LEFT || pong.ballX + BALL_SIZE >= PONG_FIELD_RIGHT) {
    pong.ballVX = -pong.ballVX;
    pong.ballX = constrain(pong.ballX, (float)PONG_FIELD_LEFT, (float)(PONG_FIELD_RIGHT - BALL_SIZE));
  }
  if (pong.ballY <= PONG_FIELD_TOP) {
    pong.ballVY = -pong.ballVY;
    pong.ballY = PONG_FIELD_TOP;
  }

  if (pong.ballVY > 0 && pong.ballY + BALL_SIZE >= PADDLE_Y && pong.ballY + BALL_SIZE <= PADDLE_Y + PADDLE_HEIGHT + 6) {
    if (pong.ballX + BALL_SIZE >= pong.paddleX && pong.ballX <= pong.paddleX + PADDLE_WIDTH) {
      float hitPos = ((pong.ballX + BALL_SIZE / 2.0) - pong.paddleX) / (float)PADDLE_WIDTH;
      pong.ballVY = -abs(pong.ballVY) * 1.03;
      pong.ballVX += (hitPos - 0.5) * 2.5;
      pong.ballVX = constrain(pong.ballVX, -6.0f, 6.0f);
      pong.ballVY = constrain(pong.ballVY, -6.0f, -1.5f);
      pong.score++;
      drawPongScore();
    }
  }

  if (pong.ballY > PONG_FIELD_BOTTOM) {
    pong.misses++;
    drawPongScore();
    pong.ballX = SCREEN_W / 2;
    pong.ballY = PONG_FIELD_TOP + 20;
    pong.ballVX = 2.2;
    pong.ballVY = 2.0;
  }

  tft.fillRect(pong.paddleX, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, COLOR_PRIMARY);
  tft.fillRect((int)pong.ballX, (int)pong.ballY, BALL_SIZE, BALL_SIZE, COLOR_TEXT);

  pong.prevPaddleX = pong.paddleX;
  pong.prevBallX = pong.ballX;
  pong.prevBallY = pong.ballY;
}
