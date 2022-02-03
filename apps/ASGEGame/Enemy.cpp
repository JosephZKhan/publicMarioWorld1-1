//
// Created by Joseph on 30/11/2021.
//

#include "Enemy.h"

Enemy::Enemy() {}
Enemy::~Enemy() {}

bool Enemy::getIsActive()
{
  return isActive;
}

void Enemy::setIsActive(bool newIsActive)
{
  isActive = newIsActive;
}

float Enemy::getSpeedX()
{
  return speedX;
}

void Enemy::setSpeedX(float newspeedX)
{
  speedX = newspeedX;
}
