//
// Created by huxy on 23/02/2020.
//

#ifndef GAMELIB_GAMECOMPONENT_HPP
#define GAMELIB_GAMECOMPONENT_HPP

class GameComponent
{
 public:
  enum class ID
  {
    INVALID_COMPONENT_ID = -1,
    NETWORK_CLIENT,
    NETWORK_SERVER,
    NUM_OF_SUPPORTED_COMPONENTS
  };

 public:
  explicit GameComponent(ID id) : id_(id) {}
  virtual ~GameComponent() = default;

  GameComponent(const GameComponent&) = default;
  GameComponent(GameComponent&&)      = default;
  GameComponent& operator=(const GameComponent&) = default;
  GameComponent& operator=(GameComponent&&) = default;

  virtual void update(double dt) = 0;
  [[nodiscard("did you not want the ID?")]] ID typeID() const;

 private:
  ID id_ = ID::INVALID_COMPONENT_ID;
};
#endif // GAMELIB_GAMECOMPONENT_HPP
