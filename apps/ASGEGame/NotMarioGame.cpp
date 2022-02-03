
#include "NotMarioGame.hpp"

//#include "soloud.h"
//#include "soloud_wav.h"

#include <Engine/FileIO.hpp>
#include <Engine/InputEvents.hpp>
#include <Engine/Keys.hpp>
#include <Engine/Logger.hpp>
#include <Engine/Sprite.hpp>
#include <Player.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

/// Initialises the game.
/// Setup your game and initialise the core components.
/// @param settings
NotMarioGame::NotMarioGame(const ASGE::GameSettings& settings) :
  OGLGame(settings),
  key_callback_id(inputs->addCallbackFnc(ASGE::E_KEY, &NotMarioGame::keyHandler, this)),
  mouse_callback_id(inputs->addCallbackFnc(ASGE::E_MOUSE_CLICK, &NotMarioGame::clickHandler, this))
{
  renderer->setClearColour(ASGE::COLOURS::CORNFLOWERBLUE);
  toggleFPS();

  // load the level's tile map

  ASGE::FILEIO::File tileMap;
  if (tileMap.open("/data/map/World_Map.tmx"))
  {
    ASGE::FILEIO::IOBuffer buffer = tileMap.read();
    std::string fileString(buffer.as_char(), buffer.length);

    tmx::Map map;
    map.loadFromString(fileString, ".");

    for (const auto& layer : map.getLayers())
    {
      if (layer->getType() == tmx::Layer::Type::Tile)
      {
        auto tileLayer = layer->getLayerAs<tmx::TileLayer>();
        for (unsigned int row = 0; row < layer->getSize().y; ++row)
        {
          for (unsigned int col = 0; col < layer->getSize().x; ++col)
          {
            auto tileInfo = tileLayer.getTiles()[row * tileLayer.getSize().x + col];
            auto tile     = map.getTilesets()[0].getTile(tileInfo.ID);
            if (tile != nullptr)
            {
              auto& sprite = tiles.emplace_back(renderer->createUniqueSprite());
              if (sprite->loadTexture(tile->imagePath))
              {
                sprite->srcRect()[0] = static_cast<float>(tile->imagePosition.x);
                sprite->srcRect()[1] = static_cast<float>(tile->imagePosition.y);
                sprite->srcRect()[2] = static_cast<float>(tile->imageSize.x);
                sprite->srcRect()[3] = static_cast<float>(tile->imageSize.y);

                sprite->width(static_cast<float>(tile->imageSize.x));
                sprite->height(static_cast<float>(tile->imageSize.y));

                sprite->scale(3);
                sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);

                sprite->yPos(static_cast<float>(row * tile->imageSize.y * 3));
                sprite->xPos(static_cast<float>(col * tile->imageSize.x * 3));
              }
            }
          }
        }
      }

      // identify layers containing solid objects and add them to collidables vector
      if (layer->getName() == "Pipes" || layer->getName() == "Ground" || layer->getName() == "Blocks")
      {
        if (layer->getType() == tmx::Layer::Type::Tile)
        {
          auto tileLayer = layer->getLayerAs<tmx::TileLayer>();
          for (unsigned int row = 0; row < layer->getSize().y; ++row)
          {
            for (unsigned int col = 0; col < layer->getSize().x; ++col)
            {
              auto tileInfo = tileLayer.getTiles()[row * tileLayer.getSize().x + col];
              auto tile     = map.getTilesets()[0].getTile(tileInfo.ID);
              if (tile != nullptr)
              {
                auto& sprite = collidables.emplace_back(renderer->createUniqueSprite());
                if (sprite->loadTexture(tile->imagePath))
                {
                  sprite->srcRect()[0] = static_cast<float>(tile->imagePosition.x);
                  sprite->srcRect()[1] = static_cast<float>(tile->imagePosition.y);
                  sprite->srcRect()[2] = static_cast<float>(tile->imageSize.x);
                  sprite->srcRect()[3] = static_cast<float>(tile->imageSize.y);

                  sprite->width(static_cast<float>(tile->imageSize.x * 3));
                  sprite->height(static_cast<float>(tile->imageSize.y * 3));

                  sprite->scale(3);
                  sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);

                  sprite->yPos(static_cast<float>(row * tile->imageSize.y * 3));
                  sprite->xPos(static_cast<float>(col * tile->imageSize.x * 3));
                }
              }
            }
          }
        }
      }

      // load the level's exit
      if (layer->getType() == tmx::Layer::Type::Object)
      {
        auto objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
        for (const auto& object : objectLayer.getObjects())
        {
          if (object.getName() == "Exit")
          {
            auto& levelExit = gameExit.emplace_back(renderer->createUniqueSprite());

            auto exitPos = object.getPosition();

            levelExit->width(41);
            levelExit->height(94);

            levelExit->scale(3);

            levelExit->xPos(exitPos.x * 3);
            levelExit->yPos(exitPos.y * 3);
          }
        }
      }
    }
  }
  // additional initialisations
  initLuigi();
  initCamera();
  initFont();
  initPowerUps();
  initGoombas();
}

/// Destroys the game.
NotMarioGame::~NotMarioGame()
{
  this->inputs->unregisterCallback(key_callback_id);
  this->inputs->unregisterCallback(mouse_callback_id);
}

/// Processes key inputs.
/// This function is added as a callback to handle the game's keyboard input.
/// For this game, calls to this function are not thread safe, so you may alter
/// the game's state but your code needs to designed to prevent data-races.
/// @param data
/// @see KeyEvent
void NotMarioGame::keyHandler(ASGE::SharedEventData data)
{
  const auto* key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

  // close window when escape key pressed
  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }

  // input handling for left/right movement

  // press A/D to walk
  if (key->key == ASGE::KEYS::KEY_D)
  {
    if (key->action == ASGE::KEYS::KEY_PRESSED)
    {
      luigi.setSpeedX(10);
    }

    if (key->action == ASGE::KEYS::KEY_RELEASED)
    {
      luigi.setSpeedX(0);
    }
  }

  if (key->key == ASGE::KEYS::KEY_A)
  {
    if (key->action == ASGE::KEYS::KEY_PRESSED)
    {
      luigi.setSpeedX(-10);
    }

    if (key->action == ASGE::KEYS::KEY_RELEASED)
    {
      luigi.setSpeedX(0);
    }
  }

  // press W to jump, if on the ground
  if (key->key == ASGE::KEYS::KEY_W)
  {
    if (key->action == ASGE::KEYS::KEY_PRESSED)
    {
      if (luigi.getIsGrounded())
      {
        luigi.setSpeedY(luigi.getJumpSpeed());
        luigi.setIsGrounded(false);
      }
    }
  }
}

/// Updates your game and all it's components using a fixed
/// timestep. This allows calculations to resolve correctly and
/// stop physics simulations from imploding
/// https://gamedev.stackexchange.com/questions/1589/when-should-i-use-a-fixed-or-variable-time-step
/// @param us
void NotMarioGame::update(const ASGE::GameTime& us)
{
  // process inputs from a connected gamePad
  gamePadInput();

  // check if the player is out of bounds
  outOfBoundsCheck();

  // process movement and collision in x axis
  playerHorizontalMovement();

  // process movement and collision in y axis
  playerVerticalMovement();

  // update the player's sprite
  playerAnimUpdate(us);

  // update the camera's position
  cameraUpdate();

  // updates related to invincibility
  invincibilityUpdate();

  // process movement and collision of enemies
  enemyHorizontalMovement();

  // update the enemy sprites
  enemyAnimUpdate(us);

  // respawn the player if they've died
  respawnFlag();
}

/// Simulates the visuals and renders your game scenes.
/// These use a variable time-step, which allows for extra smoothing and
/// interpolation based solely on the framerate of the game.
/// @param us
void NotMarioGame::render(const ASGE::GameTime& /*us*/)
{
  // render all tiles from tilemap
  for (const auto& i : tiles)
  {
    renderer->render(*i);
  }

  // render the player's sprite
  renderer->render(*luigi.sprite);

  // render the life counter
  renderer->render(lifeCounter);

  // render powerups if active
  for (unsigned int i = 0; i < powerUps.size(); ++i)
  {
    if (powerUps[i].getIsActive())
    {
      renderer->render(*powerUps[i].sprite);
    }
  }

  // render the goombas if active
  for (unsigned int i = 0; i < goombas.size(); ++i)
  {
    if (goombas[i].getIsActive())
    {
      renderer->render(*goombas[i].sprite);
    }
  }
}

/// @brief   Processes any click inputs
/// @details This function is added as a callback to handle the game's
///          mouse button input. For this game, calls to this function
///          are thread safe, so you may alter the game's state as you
///          see fit.
/// @param   data The event data relating to key input.
/// @see     ClickEvent
/// @return  void

void NotMarioGame::clickHandler(ASGE::SharedEventData data)
{
  const auto* click = dynamic_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;

  Logging::DEBUG("x_pos: " + std::to_string(x_pos));
  Logging::DEBUG("y_pos: " + std::to_string(y_pos));
}

// initialise player
bool NotMarioGame::initLuigi()
{
  // create unique pointer to sprite
  luigi.sprite = renderer->createUniqueSprite();

  // load texture to sprite
  if (!luigi.sprite->loadTexture("/data/img/mario-luigi.png"))
  {
    // report error message if texture load fails
    Logging::ERRORS("ERROR: Failed to load Luigi");
    return false;
  }

  // set the srcRect to the appropriate values on the sprite sheet
  for (unsigned int i = 0; i < 4; ++i)
  {
    luigi.sprite->srcRect()[i] = (luigi.getSmallStandingAnimationCell())[i];
  }

  // set additional attributes
  luigi.sprite->width(16);
  luigi.sprite->height(16);
  luigi.sprite->scale(3);
  luigi.sprite->setMagFilter(ASGE::GameSettings::MagFilter::NEAREST);
  luigi.sprite->xPos(150);
  luigi.sprite->yPos(575);

  return true;
}

// initialise camera
void NotMarioGame::initCamera()
{
  // set the camera's focal point to initial position
  // then assign camera to focalPoint and assign renderer to camera
  focalPoint = ASGE::Point2D{ focalPointX, focalPointY };
  camera.lookAt(ASGE::Point2D{ focalPoint });
  renderer->setProjectionMatrix(camera.getView());
}

// returns true if either collPoint Point2D is inside the target sprite
// used to calculate collision in a given direction, with two forward-facing collPoints
bool NotMarioGame::directionalCollisionCheck(
  const ASGE::Sprite* target, std::array<ASGE::Point2D, 2> collPoints)
{
  // iterate through points array
  for (unsigned int i = 0; i < 2; ++i)
  {
    if (collPoints[i].x > target->xPos())
    {
      if (collPoints[i].x < target->xPos() + target->width())
      {
        if (collPoints[i].y > target->yPos())
        {
          if (collPoints[i].y < target->yPos() + target->height())
          {
            return true;
          }
        }
      }
    }
  }
  return false;
}

// process data coming from gamePad
void NotMarioGame::gamePadInput()
{
  // gamePad is the player 1 gamePad
  auto gamePad = inputs->getGamePad(0);

  // presence check for gamepad connection
  if (gamePad.is_connected)
  {
    // update speed if left analogue stick is outside dead zone
    if (std::abs(gamePad.axis[0]) > 0.1)
    {
      luigi.setSpeedX((gamePad.axis[0]) * 8);
    }

    else
    {
      luigi.setSpeedX(0);
    }

    // jump if the a button is pressed (and the player is grounded)
    if (gamePad.buttons[0] != 0u)
    {
      if (luigi.getIsGrounded())
      {
        luigi.setSpeedY(luigi.getJumpSpeed());
      }
    }
  }
}

void NotMarioGame::outOfBoundsCheck()
{
  // kill player if player falls out of bounds
  if (luigi.sprite->yPos() > 1280)
  {
    death();
  }
}

void NotMarioGame::playerHorizontalMovement()
{
  // if player is walking left or right
  if (static_cast<int>(luigi.getSpeedX()) != 0)
  {
    // assume they can move freely
    bool canMoveInX = true;

    // calculate position on next frame
    float newXPos = luigi.sprite->xPos() + static_cast<float>(luigi.getSpeedX());

    // calculate the positions of player sprite vertices at newXPos
    ASGE::Point2D newTopRight;
    ASGE::Point2D newBottomRight;
    ASGE::Point2D newTopLeft;
    ASGE::Point2D newBottomLeft;

    newTopRight    = { newXPos + (luigi.sprite->width() * 3), luigi.sprite->yPos() };
    newBottomRight = { newXPos + (luigi.sprite->width() * 3),
                       (luigi.sprite->yPos() + luigi.sprite->height()) };
    newTopLeft     = { newXPos, luigi.sprite->yPos() };
    newBottomLeft  = { newXPos, (luigi.sprite->yPos() + luigi.sprite->height()) };

    // add the two forward-facing vertices in the player's horizontal direction to newPoints array
    std::array<ASGE::Point2D, 2> newPoints;

    if (luigi.getSpeedX() > 0)
    {
      newPoints[0] = newTopRight;
      newPoints[1] = newBottomRight;
    }

    if (luigi.getSpeedX() < 0)
    {
      newPoints[0] = newTopLeft;
      newPoints[1] = newBottomLeft;
    }

    // iterate through collidables list
    // inhibit player's horizontal movement if they collide with anything solid
    for (const auto& i : collidables)
    {
      if (directionalCollisionCheck(i.get(), newPoints))
      {
        canMoveInX = false;
      }
    }

    // trigger victory if the player collides with the exit
    for (const auto& i : gameExit)
    {
      if (directionalCollisionCheck(i.get(), newPoints))
      {
        victory();
      }
    }

    // power-up player if they collect a powerup
    for (auto& i : powerUps)
    {
      if (directionalCollisionCheck(i.sprite.get(), newPoints))
      {
        if (i.getIsActive())
        {
          i.setIsActive(false);
          if (!(luigi.getIsPoweredUp()))
          {
            powerUp();
          }
        }
      }
    }

    // if player walks into an active goomba and isn't invincible
    // kill/depower them
    for (auto& i : goombas)
    {
      if (directionalCollisionCheck(i.sprite.get(), newPoints))
      {
        if (i.getIsActive())
        {
          if (!luigi.getIsInvincible())
          {
            if (!luigi.getIsPoweredUp())
            {
              death();
            }
            else
            {
              powerDown();
            }
          }
        }
      }
    }

    // only continue with collision check if the player hasn't been killed
    if (luigi.getIsAlive())

    { // inhibit horizontal movement if player tries backtracking past the left of the window
      if (luigi.getSpeedX() < 0)
      {
        if (newXPos < backOfScreen)
        {
          canMoveInX = false;
        }
      }

      // cancel out horizontal movement if player unable to move horizontally
      if (!canMoveInX)
      {
        newXPos -= luigi.getSpeedX();
      }

      // apply horizontal movement to the player sprite
      luigi.sprite->xPos(newXPos);
    }
  }
}

void NotMarioGame::playerVerticalMovement()
{
  // if the player is moving up or down
  if (static_cast<int>(luigi.getSpeedY()) != 0)
  {
    // assume the player can move freely
    bool canMoveInY = true;

    // calculate position on next frame
    float newYPos = luigi.sprite->yPos() + static_cast<float>(luigi.getSpeedY());

    // calculate the positions of player sprite vertices at newXPos
    ASGE::Point2D newTopRight;
    ASGE::Point2D newBottomRight;
    ASGE::Point2D newTopLeft;
    ASGE::Point2D newBottomLeft;

    newTopRight    = { luigi.sprite->xPos() + (luigi.sprite->width()), newYPos };
    newBottomRight = { luigi.sprite->xPos() + (luigi.sprite->width()),
                       (newYPos + (luigi.sprite->height()) * 3) };
    newTopLeft     = { luigi.sprite->xPos(), newYPos };
    newBottomLeft  = { luigi.sprite->xPos(), (newYPos + (luigi.sprite->height()) * 3) };

    // add the two forward-facing vertices in the player's horizontal direction to newPoints array
    std::array<ASGE::Point2D, 2> newPoints;

    if (luigi.getSpeedY() > 0)
    {
      newPoints[0] = newBottomLeft;
      newPoints[1] = newBottomRight;
    }

    if (luigi.getSpeedY() < 0)
    {
      newPoints[0] = newTopLeft;
      newPoints[1] = newTopRight;
    }

    // iterate through collidables list
    // inhibit player's vertical movement if they collide with anything solid
    for (const auto& i : collidables)
    {
      if (directionalCollisionCheck(i.get(), newPoints))
      {
        canMoveInY = false;
      }
    }

    // trigger victory if the player collides with the exit
    for (const auto& i : gameExit)
    {
      if (directionalCollisionCheck(i.get(), newPoints))
      {
        victory();
      }
    }

    // power-up player if they collide with an active powerup
    for (auto& i : powerUps)
    {
      if (directionalCollisionCheck(i.sprite.get(), newPoints))
      {
        if (i.getIsActive())
        {
          i.setIsActive(false);
          if (!(luigi.getIsPoweredUp()))
          {
            powerUp();
          }
        }
      }
    }

    // kill goomba and bounce player upwards if player jumps on goomba
    for (auto& i : goombas)
    {
      if (directionalCollisionCheck(i.sprite.get(), newPoints))
      {
        if (luigi.getSpeedY() > 0)
        {
          if (i.getIsActive())
          {
            i.setIsActive(false);
            luigi.setSpeedY(luigi.getJumpSpeed());
          }
        }
      }
    }

    // cancel out vertical movement if player unable to move vertically
    if (!canMoveInY)
    {
      newYPos -= luigi.getSpeedY();

      // update isGrounded status if player is colliding with floor
      if (luigi.getSpeedY() > 0)
      {
        luigi.setIsGrounded(true);
      }

      // bounce player downwards if they collide with something above them
      if (luigi.getSpeedY() < 0)
      {
        luigi.setSpeedY(static_cast<float>(0.200003));
      }
    }

    // update isGrounded status
    else
    {
      luigi.setIsGrounded(false);
    }

    // apply vertical movement to player sprite
    luigi.sprite->yPos(newYPos);
  }

  // update player's vertical speed
  luigi.gravityUpdate();
}

// update the player's animation frame
void NotMarioGame::playerAnimUpdate(const ASGE::GameTime& us)
{
  // if player is touching floor
  if (luigi.getIsGrounded())
  {
    // if player is walking
    if (static_cast<int>(luigi.getSpeedX()) != 0)
    {
      // stabilise animation with framerate
      animTimer += static_cast<float>(us.deltaInSecs());
      if (animTimer > ANIMATION_FRAME_RATE)
      {
        // increment animation index, reset to 0 if it exceeds 3
        animTimer = 0.0F;
        luigi.setAnimIdx(luigi.getAnimIdx() + 1);
        if (luigi.getAnimIdx() > 3)
        {
          luigi.setAnimIdx(0);
        }
      }

      // set sprite rect to the appropriate frame of the running animation
      for (unsigned int i = 0; i < 4; ++i)
      {
        unsigned long long a = static_cast<unsigned long long>(luigi.getAnimIdx());
        if (!luigi.getIsPoweredUp())
        {
          luigi.sprite->srcRect()[i] = (luigi.getSmallRunAnimationCells()[a])[i];
        }
        else
        {
          luigi.sprite->srcRect()[i] = (luigi.getBigRunAnimationCells()[a])[i];
        }
      }
    }

    // otherwise, set sprite rect to standing sprite
    else
    {
      for (unsigned int i = 0; i < 4; ++i)
      {
        if (!luigi.getIsPoweredUp())
        {
          luigi.sprite->srcRect()[i] = luigi.getSmallStandingAnimationCell()[i];
        }
        else
        {
          luigi.sprite->srcRect()[i] = luigi.getBigStandingAnimationCell()[i];
        }
      }
    }
  }

  // if player is in the air, set sprite to jumping
  else
  {
    for (unsigned int i = 0; i < 4; ++i)
    {
      if (!luigi.getIsPoweredUp())
      {
        luigi.sprite->srcRect()[i] = luigi.getSmallJumpAnimationCell()[i];
      }
      else
      {
        luigi.sprite->srcRect()[i] = luigi.getBigJumpAnimationCell()[i];
      }
    }
  }

  // flip player sprite on x axis in response to player's direction
  if (luigi.getSpeedX() > 0)
  {
    if (luigi.sprite->isFlippedOnX())
    {
      luigi.sprite->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
    }
  }
  if (luigi.getSpeedX() < 0)
  {
    if (!(luigi.sprite->isFlippedOnX()))
    {
      luigi.sprite->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
    }
  }
}

void NotMarioGame::cameraUpdate()
{
  // if the player moves right beyond a certain threshold
  if (luigi.sprite->xPos() > cameraBoundX and luigi.getSpeedX() > 0)
  {
    // scroll the camera, the threshold boundary, and the back of the screen
    focalPointX += luigi.getSpeedX();
    cameraBoundX += luigi.getSpeedX();
    backOfScreen += luigi.getSpeedX();

    // update the camera to look at the new focal point
    focalPoint = ASGE::Point2D{ focalPointX, 360.0F };
    camera.lookAt(focalPoint);

    lifeCounter.setPositionX(lifeCounter.getPosition().x + luigi.getSpeedX());
  }

  renderer->setProjectionMatrix(camera.getView());
}

// subtract 1 from lives counter, spawn player or trigger game over if player is out of lives
void NotMarioGame::death()
{
  if (luigi.getIsAlive())
  {
    luigi.setIsAlive(false);
    luigi.setLives(luigi.getLives() - 1);
    if (luigi.getLives() < 0)
    {
      gameOver();
    }
  }
}

// reset the level after player loses a life
void NotMarioGame::spawn()
{
  // send luigi's sprite back to start position
  luigi.sprite->xPos(150);
  luigi.sprite->yPos(575);

  // reset camera attributes back to start of level
  focalPointX  = 640;
  focalPointY  = 360;
  cameraBoundX = 400;
  backOfScreen = 0;

  // set the camera's focal point to initial position
  // then assign camera to focalPoint and assign renderer to camera
  focalPoint = ASGE::Point2D{ focalPointX, focalPointY };
  camera.lookAt(ASGE::Point2D{ focalPoint });
  renderer->setProjectionMatrix(camera.getView());

  // update lives counter
  lifeCounter.setString("LIVES: " + std::to_string(luigi.getLives()));
  lifeCounter.setPositionX(125);

  // power down if necessary
  if (luigi.getIsPoweredUp())
  {
    powerDown();
  }

  // respawn any powerups
  for (auto& i : powerUps)
  {
    i.setIsActive(true);
  }

  // respawn any goombas
  for (auto& i : goombas)
  {
    i.setIsActive(true);
  }

  luigi.setIsAlive(true);
}

// initialise fonts
void NotMarioGame::initFont()
{
  // load font for the lives counter, assign attributes
  if (auto idx = renderer->loadFont("/data/fonts/SuperPlumberBrothers-y00V.ttf", 48); idx > 0)
  {
    fontIdx = idx;
    lifeCounter.setFont(renderer->getFont(fontIdx))
      .setString("LIVES: 3")
      .setPosition({ 125, 100 })
      .setColour(ASGE::COLOURS::WHITESMOKE);
  }
}

// called when the level is completed
void NotMarioGame::victory()
{
  std::cout << "THANK YOU MARIO!\nBUT YOUR PRINCESS IS IN ANOTHER CASTLE.\n";
  signalExit();
}

// called when the player runs out of lives
void NotMarioGame::gameOver()
{
  std::cout << "GAME OVER.\n";
  signalExit();
}

// initialise power ups
bool NotMarioGame::initPowerUps()
{
  // render all tiles from tilemap
  for (auto& powerUp : powerUps)
  {
    powerUp.sprite = renderer->createUniqueSprite();

    if (!powerUp.sprite->loadTexture("/data/img/super-mushroom.png"))
    {
      // report error message if texture load fails
      Logging::ERRORS("ERROR: Failed to load Super Mushroom");
      return false;
    }

    powerUp.sprite->srcRect()[0] = 0;
    powerUp.sprite->srcRect()[1] = 0;
    powerUp.sprite->srcRect()[2] = 16;
    powerUp.sprite->srcRect()[3] = 16;

    powerUp.sprite->width(16);
    powerUp.sprite->height(16);
    powerUp.sprite->scale(3);
    powerUp.sprite->setMagFilter(ASGE::GameSettings::MagFilter::NEAREST);
  }

  powerUps[0].sprite->xPos(5830);
  powerUps[0].sprite->yPos(190);

  powerUps[1].sprite->xPos(1060);
  powerUps[1].sprite->yPos(375);

  return true;
}

// called when player collects power-up
void NotMarioGame::powerUp()
{
  luigi.setIsPoweredUp(true);
  luigi.sprite->yPos(luigi.sprite->yPos() - 48);
  luigi.sprite->height(32);
}

// called when player loses power-up
void NotMarioGame::powerDown()
{
  luigi.setIsPoweredUp(false);
  luigi.sprite->height(16);
  luigi.setIsInvincible(true);
}

// initialise goombas vector
bool NotMarioGame::initGoombas()
{
  // create unique pointers for each goomba sprite
  for (auto& goomba : goombas)
  {
    goomba.sprite = renderer->createUniqueSprite();

    if (!goomba.sprite->loadTexture("/data/img/enemies.png"))
    {
      // report error message if texture load fails
      Logging::ERRORS("ERROR: Failed to load Goomba");
      return false;
    }

    // initialise texture and size
    goomba.sprite->srcRect()[0] = 0;
    goomba.sprite->srcRect()[1] = 4;
    goomba.sprite->srcRect()[2] = 16;
    goomba.sprite->srcRect()[3] = 16;

    goomba.sprite->width(21);
    goomba.sprite->height(21);
    goomba.sprite->scale(4);
    goomba.sprite->setMagFilter(ASGE::GameSettings::MagFilter::NEAREST);
  }

  // set individual goomba positions
  goombas[0].sprite->xPos(1544);
  goombas[0].sprite->yPos(550);

  goombas[1].sprite->xPos(5468);
  goombas[1].sprite->yPos(550);

  goombas[2].sprite->xPos(8373);
  goombas[2].sprite->yPos(550);

  return true;
}

// respawn player if they aren't alive
void NotMarioGame::respawnFlag()
{
  if (!luigi.getIsAlive())
  {
    spawn();
  }
}

// update player's invincibility status
void NotMarioGame::invincibilityUpdate()
{
  if (luigi.getIsInvincible())
  {
    // when invincible, set player to translucent
    luigi.sprite->opacity(0.5);

    // decrement invincibility timer
    luigi.setInvincibleTimer(luigi.getInvincibleTimer() - 1);

    // when timer hits 0, remove invincibility status and reset timer
    if (luigi.getInvincibleTimer() == 0)
    {
      luigi.setIsInvincible(false);
      luigi.setInvincibleTimer(150);
      luigi.sprite->opacity(1);
    }
  }
}

void NotMarioGame::enemyHorizontalMovement()
{
  for (auto& i : goombas)
  {
    if (i.getIsActive())
    {
      // calculate position on next frame
      float newXPos = i.sprite->xPos() + static_cast<float>(i.getSpeedX());

      // calculate the positions of player sprite vertices at newXPos
      ASGE::Point2D newTopRight;
      ASGE::Point2D newBottomRight;
      ASGE::Point2D newTopLeft;
      ASGE::Point2D newBottomLeft;

      newTopRight    = { newXPos + (i.sprite->width() * 3), i.sprite->yPos() };
      newBottomRight = { newXPos + (i.sprite->width() * 3),
                         (i.sprite->yPos() + i.sprite->height()) };
      newTopLeft     = { newXPos, i.sprite->yPos() };
      newBottomLeft  = { newXPos, (i.sprite->yPos() + i.sprite->height()) };

      // add the two forward-facing vertices in the player's horizontal direction to newPoints array
      std::array<ASGE::Point2D, 2> newPoints;

      if (i.getSpeedX() > 0)
      {
        newPoints[0] = newTopRight;
        newPoints[1] = newBottomRight;
      }

      if (i.getSpeedX() < 0)
      {
        newPoints[0] = newTopLeft;
        newPoints[1] = newBottomLeft;
      }

      // iterate through collidables list
      // make goombas turn around if they collide with a collidable
      for (const auto& j : collidables)
      {
        if (directionalCollisionCheck(j.get(), newPoints))
        {
          i.setSpeedX(-(i.getSpeedX()));
        }
      }

      if (directionalCollisionCheck(luigi.sprite.get(), newPoints))
      {
        if (i.getIsActive())
        {
          if (!luigi.getIsInvincible())
          {
            if (!luigi.getIsPoweredUp())
            {
              death();
            }
            else
            {
              powerDown();
            }
          }
        }
      }

      // apply horizontal movement to the player sprite
      i.sprite->xPos(newXPos);
    }
  }
}

void NotMarioGame::enemyAnimUpdate(const ASGE::GameTime& us)
{
  for (const auto& i : goombas)
  { // stabilise animation with framerate
    animTimer += static_cast<float>(us.deltaInSecs());
    if (animTimer > ANIMATION_FRAME_RATE)
    {
      // keep flipping the sprite horizontally
      animTimer = 0.0F;
      if (!(i.sprite->isFlippedOnX()))
      {
        i.sprite->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
      }
      else
      {
        i.sprite->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
      }
    }
  }
}
