//
// Created by Joseph on 29/11/2021.
//

#ifndef NOTMARIOGAME_POWERUP_H
#define NOTMARIOGAME_POWERUP_H

#pragma once

#include <ASGEGameLib/GComponent.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <random>

#include <vector>

class PowerUp
{
 public:
  PowerUp();
  ~PowerUp();
  std::unique_ptr<ASGE::Sprite> sprite = nullptr;
  bool getIsActive();
  void setIsActive(bool newIsActive);

 private:
  bool isActive = true;
};

#endif // NOTMARIOGAME_POWERUP_H
