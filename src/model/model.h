#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <string>

using namespace std;

class Model
{
public:
  uint id;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

  Model(uint id)
  {
    id = id;
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
  };

  virtual ~Model() = default;
  virtual void draw() = 0;
  virtual void update(float deltaTime) = 0;

  // getters
  glm::vec3 getId() { return id; };
  glm::vec3 getPosition() { return position; };
  glm::vec3 getRotation() { return rotation; };
  glm::vec3 getScale() { return scale; };

  // setters
  void setId(glm::vec3 newId) { id = newId; };
  void setPosition(glm::vec3 position) { position = position; };
  void setRotation(glm::vec3 rotation) { rotation = rotation; };
  void setScale(glm::vec3 scale) { scale = scale; };
};

#endif