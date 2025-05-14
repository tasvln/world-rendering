#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO: split camera's -> Orbit Camera & FPS Camera & TPS Camera
class OrbitCamera
{
public:
  // Basic Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  // Orbit Attributes
  glm::vec3 Target;

  // Euler Angles
  float Yaw;
  float Pitch;

  // Orbit Attributes
  float Radius;

  // Basic Camera Options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // Constructor with default values
  OrbitCamera(
      glm::vec3 target = glm::vec3(0.0f),
      float radius = 10.0f,
      float yaw = -90.0f,
      float pitch = 0.0f)
  {
    Target = target;
    Radius = radius;
    Yaw = yaw;
    Pitch = pitch;

    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    MouseSensitivity = 0.1f;
    Zoom = 45.0f;
    MovementSpeed = 10.0f;

    // Will calculate Position from Target, Yaw, Pitch, and Radius
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix()
  {
    return glm::lookAt(Position, Target, Up);
  }

  void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
  {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Constrain pitch to prevent flip
    if (constrainPitch)
    {
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    updateCameraVectors();
  }

  void processMouseScroll(float yoffset)
  {
    Radius -= yoffset;
    if (Radius < 1.0f)
      Radius = 1.0f;
    if (Radius > 100.0f)
      Radius = 100.0f;

    updateCameraVectors();
  }

  void setTarget(glm::vec3 newTarget)
  {
    Target = newTarget;
    updateCameraVectors();
  }

private:
  void updateCameraVectors()
  {
    // Orbit spherical coordinates to Cartesian
    float x = Radius * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    float y = Radius * sin(glm::radians(Pitch));
    float z = Radius * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

    Position = Target + glm::vec3(x, y, z);

    Front = glm::normalize(Target - Position);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif
