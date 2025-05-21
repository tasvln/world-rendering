#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <string>

using namespace std;

namespace mod
{
  class Model
  {
  public:
    string filePath;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Model(const string &filePath)
    {
      this->filePath = filePath;
      position = glm::vec3(0.0f);
      rotation = glm::vec3(0.0f);
      scale = glm::vec3(1.0f);
    };

    virtual ~Model() = default;
    virtual void draw() = 0;
    virtual void update(float deltaTime) = 0;

    // getters
    string getFilePath() { return filePath; };
    glm::vec3 getPosition() { return position; };
    glm::vec3 getRotation() { return rotation; };
    glm::vec3 getScale() { return scale; };

    // setters
    void setFilePath(string filePath) { this->filePath = filePath; };
    void setPosition(glm::vec3 position) { position = position; };
    void setRotation(glm::vec3 rotation) { rotation = rotation; };
    void setScale(glm::vec3 scale) { scale = scale; };
  };
}
#endif