#include "NotMarioGame.hpp"
#include <Engine/GameSettings.hpp>
#include <Engine/Logger.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
  ASGE::GameSettings game_settings;
  game_settings.window_title  = "NotMarioGame";
  game_settings.window_width  = 1280;
  game_settings.window_height = 720;
  game_settings.mode          = ASGE::GameSettings::WindowMode::WINDOWED;
  game_settings.fixed_ts      = 30;
  game_settings.fps_limit     = 120;

  Logging::INFO("Launching Game!");
  NotMarioGame game(game_settings);
  return game.run();
}
