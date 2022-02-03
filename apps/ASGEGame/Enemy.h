//
// Created by Joseph on 30/11/2021.
//

#ifndef NOTMARIOGAME_ENEMY_H
#define NOTMARIOGAME_ENEMY_H

#pragma once

#include <ASGEGameLib/GComponent.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <random>

#include <vector>

class Enemy
{
 public:
  Enemy();
  ~Enemy();
  std::unique_ptr<ASGE::Sprite> sprite = nullptr;
  bool getIsActive();
  void setIsActive(bool newIsActive);
  float getSpeedX();
  void setSpeedX(float newSpeedX);

 private:
  bool isActive = true;
  float speedX  = 5.0F;
};

#endif // NOTMARIOGAME_ENEMY_H
