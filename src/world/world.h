#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H

#include <glm/glm.hpp>

#include "../model/model.h"

namespace nsi
{
  class World : public Model
  {
  public:
    World(const std::string &filePath) : Model(filePath)
    {
      // Initialize the world with the given file path
    }
    ~World() override
    {
      // Clean up resources if needed
    }
    void draw() override
    {
      // Implement the draw logic for the world
    }
    void update(float deltaTime) override
    {
      // Implement the update logic for the world
    }

  private:
  };
}

#endif