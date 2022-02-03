//
// Created by Joseph on 29/11/2021.
//

#include "PowerUp.h"

PowerUp::PowerUp() {}
PowerUp::~PowerUp() {}

bool PowerUp::getIsActive()
{
  return isActive;
}

void PowerUp::setIsActive(bool newIsActive)
{
  isActive = newIsActive;
}
