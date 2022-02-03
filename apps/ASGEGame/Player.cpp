//
// Created by Joseph on 29/10/2021.
//

#include "Player.h"

Player::Player()
{
  smallStandingAnimationCell = { 176, 128, 16, 16 };

  smallRunAnimationCells[0] = { 80, 128, 16, 16 };
  smallRunAnimationCells[1] = { 96, 128, 16, 16 };
  smallRunAnimationCells[2] = { 112, 128, 16, 16 };
  smallRunAnimationCells[3] = smallRunAnimationCells[1];

  smallJumpAnimationCell = { 144, 128, 16, 16 };

  bigStandingAnimationCell = { 176, 96, 16, 32 };

  bigRunAnimationCells[0] = { 80, 96, 16, 32 };
  bigRunAnimationCells[1] = { 96, 96, 16, 32 };
  bigRunAnimationCells[2] = { 112, 96, 16, 32 };
  bigRunAnimationCells[3] = bigRunAnimationCells[1];

  bigJumpAnimationCell = { 144, 96, 16, 32 };
}

Player::~Player() {}

float Player::getSpeedX()
{
  return speedX;
}

void Player::setSpeedX(float newspeedX)
{
  speedX = newspeedX;
}

float Player::getSpeedY()
{
  return speedY;
}

void Player::setSpeedY(float newspeedY)
{
  speedY = newspeedY;
}

std::array<float, 4> Player::getSmallStandingAnimationCell()
{
  return smallStandingAnimationCell;
}

std::array<std::array<float, 4>, 4> Player::getSmallRunAnimationCells()
{
  return smallRunAnimationCells;
}

std::array<float, 4> Player::getSmallJumpAnimationCell()
{
  return smallJumpAnimationCell;
}

std::array<float, 4> Player::getBigStandingAnimationCell()
{
  return bigStandingAnimationCell;
}

std::array<std::array<float, 4>, 4> Player::getBigRunAnimationCells()
{
  return bigRunAnimationCells;
}

std::array<float, 4> Player::getBigJumpAnimationCell()
{
  return bigJumpAnimationCell;
}

float Player::getJumpSpeed()
{
  return jumpSpeed;
}

bool Player::getIsGrounded()
{
  return isGrounded;
}

void Player::setIsGrounded(bool newIsGrounded)
{
  isGrounded = newIsGrounded;
}

void Player::gravityUpdate()
{
  // accelerate the player downwards, unless their falling speed surpasses a given threshold
  if (speedY + grav < maxSpeedY)
  {
    speedY += grav;
  }
}

int Player::getAnimIdx()
{
  return animIdx;
}

void Player::setAnimIdx(int newAnimIdx)
{
  animIdx = newAnimIdx;
}

int Player::getLives()
{
  return lives;
}

void Player::setLives(int newLives)
{
  lives = newLives;
}

bool Player::getIsPoweredUp()
{
  return isPoweredUp;
}

void Player::setIsPoweredUp(bool newIsPoweredUp)
{
  isPoweredUp = newIsPoweredUp;
}

bool Player::getIsAlive()
{
  return isAlive;
}

void Player::setIsAlive(bool newIsAlive)
{
  isAlive = newIsAlive;
}

bool Player::getIsInvincible()
{
  return isInvincible;
}

void Player::setIsInvincible(bool newIsInvincible)
{
  isInvincible = newIsInvincible;
}

int Player::getInvincibleTimer()
{
  return invincibleTimer;
}

void Player::setInvincibleTimer(int newInvinvibleTimer)
{
  invincibleTimer = newInvinvibleTimer;
}
