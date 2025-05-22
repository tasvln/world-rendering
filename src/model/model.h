#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <string>

using namespace std;

namespace mdl
{
  class Model
  {
  public:
    string filePath;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Model(const string &filePath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
    {
      this->filePath = filePath;
      this->position = position;
      this->rotation = rotation;
      this->scale = scale;
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

    glm::mat4 getModelMatrix() const
    {
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, position);
      mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
      mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
      mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
      mat = glm::scale(mat, scale);
      return mat;
    }
  };
}
#endif