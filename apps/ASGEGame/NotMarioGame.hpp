#pragma once

#include <ASGEGameLib/GComponent.hpp>
#include <Enemy.h>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <Player.h>
#include <PowerUp.h>
#include <random>
#include <tmxlite/Layer.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <vector>

class NotMarioGame : public ASGE::OGLGame
{
 public:
  explicit NotMarioGame(const ASGE::GameSettings& settings);
  ~NotMarioGame() override;
  NotMarioGame(const NotMarioGame&) = delete;
  NotMarioGame& operator=(const NotMarioGame&) = delete;

 private:
  void keyHandler(ASGE::SharedEventData data);
  void clickHandler(ASGE::SharedEventData data);

  void update(const ASGE::GameTime& us) override;
  void render(const ASGE::GameTime& us) override;

  bool initLuigi();
  void initCamera();
  void initFont();
  bool initPowerUps();
  bool initGoombas();
  bool
  directionalCollisionCheck(const ASGE::Sprite* target, std::array<ASGE::Point2D, 2> collPoints);

  void gamePadInput();
  void outOfBoundsCheck();
  void playerHorizontalMovement();
  void playerVerticalMovement();
  void playerAnimUpdate(const ASGE::GameTime& us);
  void cameraUpdate();
  void death();
  void spawn();
  void victory();
  void gameOver();
  void powerUp();
  void powerDown();
  void respawnFlag();
  void invincibilityUpdate();
  void enemyHorizontalMovement();
  void enemyAnimUpdate(const ASGE::GameTime& us);

 private:
  std::vector<std::unique_ptr<GameComponent>> game_components;
  std::string key_callback_id{};   /**< Key Input Callback ID. */
  std::string mouse_callback_id{}; /**< Mouse Input Callback ID. */
  std::vector<std::unique_ptr<ASGE::Sprite>> tiles;
  std::vector<std::unique_ptr<ASGE::Sprite>> collidables;
  std::vector<std::unique_ptr<ASGE::Sprite>> gameExit;
  std::vector<PowerUp> powerUps{ 2 };
  std::vector<Enemy> goombas{ 3 };
  Player luigi;
  float backOfScreen = 0;
  float cameraBoundX = 400;
  float focalPointX  = 640;
  float focalPointY  = 360;
  ASGE::Camera camera{ 1280, 720 };
  ASGE::Point2D focalPoint{};
  float animTimer             = 0;
  double ANIMATION_FRAME_RATE = 0.1;
  int fontIdx                 = 0;
  ASGE::Text lifeCounter      = {};
};
