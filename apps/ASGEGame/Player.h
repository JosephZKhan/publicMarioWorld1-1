//
// Created by Joseph on 29/10/2021.
//

#ifndef NOTMARIOGAME_PLAYER_H
#define NOTMARIOGAME_PLAYER_H

#pragma once

#include <ASGEGameLib/GComponent.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <random>

#include <vector>

class Player
{
 public:
  Player();
  ~Player();
  float getSpeedX();
  void setSpeedX(float newSpeedX);
  float getSpeedY();
  void setSpeedY(float newSpeedY);
  std::unique_ptr<ASGE::Sprite> sprite = nullptr;
  std::array<float, 4> getSmallStandingAnimationCell();
  std::array<std::array<float, 4>, 4> getSmallRunAnimationCells();
  std::array<float, 4> getSmallJumpAnimationCell();
  std::array<float, 4> getBigStandingAnimationCell();
  std::array<std::array<float, 4>, 4> getBigRunAnimationCells();
  std::array<float, 4> getBigJumpAnimationCell();
  float getJumpSpeed();
  bool getIsGrounded();
  void setIsGrounded(bool newIsGrounded);
  void gravityUpdate();
  int getAnimIdx();
  void setAnimIdx(int newAnimIdx);
  int getLives();
  void setLives(int newLives);
  bool getIsPoweredUp();
  void setIsPoweredUp(bool newIsPoweredUp);
  bool getIsAlive();
  void setIsAlive(bool newIsAlive);
  bool getIsInvincible();
  void setIsInvincible(bool newIsInvincible);
  int getInvincibleTimer();
  void setInvincibleTimer(int newInvinvibleTimer);

 private:
  float speedX = 0.0F;
  float speedY = 1.0F;
  std::array<float, 4> smallStandingAnimationCell;
  std::array<std::array<float, 4>, 4> smallRunAnimationCells;
  std::array<float, 4> smallJumpAnimationCell;
  std::array<float, 4> bigStandingAnimationCell;
  std::array<std::array<float, 4>, 4> bigRunAnimationCells;
  std::array<float, 4> bigJumpAnimationCell;
  float jumpSpeed     = -22.0F;
  bool isGrounded     = true;
  float maxSpeedY     = 20.0F;
  float grav          = 1.01F;
  int animIdx         = 0;
  int lives           = 3;
  bool isPoweredUp    = false;
  bool isAlive        = true;
  bool isInvincible   = false;
  int invincibleTimer = 150;
};

#endif // NOTMARIOGAME_PLAYER_H
