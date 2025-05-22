#ifndef ASSIMP_MODEL_H
#define ASSIMP_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../model/model.h"

using namespace mdl;

namespace aml
{
  class AssimpModel : public Model
  {
  public
    AssimpModel(const string &filePath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f)) : Model(filePath, position, rotation, scale) {};
    ~AssimpModel() override = default;
  };
}

#endif