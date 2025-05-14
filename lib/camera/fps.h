#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO: split camera's -> Orbit Camera & FPS Camera & TPS Camera

enum CameraMovement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

class FPSCamera
{
public:
  // Basic Camera Attributes
  // where you are
  glm::vec3 position;
  // where you're looking
  glm::vec3 front;
  // what is considered 'up'
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  glm::vec3 velocity;

  // Euler Angles
  float yaw;
  float pitch;

  // Movement
  float speed;
  float sensitivity;

  // Others
  bool isGrounded;
  bool isFlying;

  const float gravity = -9.81f;
  const float jumpForce = 5.0f;

  const float standHeight = 1.0f;
  const float crouchOffset = 0.2f;

  // Constructor with default values
  FPSCamera(glm::vec3 startPosition, glm::vec3 upDirection, glm::vec3 velocity, float startYaw, float startPitch)
  {
    position = startPosition;
    worldUp = upDirection;
    yaw = startYaw;
    pitch = startPitch;

    speed = 10.0f;
    sensitivity = 0.1f;

    velocity = velocity;
    isGrounded = true;
    isFlying = false;

    updateCameraVectors();
  }

  glm::mat4 getViewMatrix()
  {
    return glm::lookAt(position, position + front, up);
  }

  void jump()
  {
    if (isGrounded && !isFlying)
    {
      velocity.y = jumpForce;
      isGrounded = false;
    }
  }

  void toggleFly()
  {
    isFlying = !isFlying;
    if (isFlying)
    {
      velocity.y = 0.0f; // cancel any fall
      isGrounded = false;
    }
  }

  void updatePhysics(float deltaTime)
  {
    if (!isFlying)
    {
      velocity.y += gravity * deltaTime;
      position.y += velocity.y * deltaTime;

      if (position.y <= standHeight)
      {
        position.y = standHeight;
        velocity.y = 0.0f;
        isGrounded = true;
      }
    }
  }

  void processMouseMovement(float xoffset, float yoffset)
  {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    updateCameraVectors();
  }

  void processKeyboard(const char *direction, float deltaTime)
  {
    float velocity = speed * deltaTime;

    glm::vec3 move = glm::vec3(0.0f);

    if (strcmp(direction, "FORWARD") == 0)
      move += isFlying ? front : glm::vec3(front.x, 0.0f, front.z);
    if (strcmp(direction, "BACKWARD") == 0)
      move -= isFlying ? front : glm::vec3(front.x, 0.0f, front.z);
    if (strcmp(direction, "LEFT") == 0)
      move -= right;
    if (strcmp(direction, "RIGHT") == 0)
      move += right;

    position += glm::normalize(move) * velocity;

    // Lock to ground plane
    // position.y = standHeight;
  }

private:
  void updateCameraVectors()
  {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};

#endif
