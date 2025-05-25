#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H

#include <glm/glm.hpp>

#include "../../assimpModel/AssimpModel.h"

namespace nsi
{
  class World : public AssimpModel
  {
  public:
    World(const std::string &filePath) : AssimpModel(filePath) {};
    World(const std::string &filePath, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : AssimpModel(filePath, position, rotation, scale) {};
    ~World() override = default;
    void draw(GLuint shader) override
    {
      AssimpModel::draw(shader);
    }
  };
}

#endif