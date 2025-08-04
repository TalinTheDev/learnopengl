#ifndef CAMERA_H
#define CAMERA_H

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

enum Direction {
  Forward,
  Backward,
  Left,
  Right,
  Up,
  Down,
};

class Camera {
public:
  float fov = 45.0f;
  float moveSpeed = 2.5f;
  float lookSens = 0.1f;

  float yaw = -77.0f;
  float pitch = -25.0f;

  glm::vec3 position = glm::vec3(0.0f, 2.0f, 4.0f);
  glm::vec3 frontFace = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);

  Camera() {
    // Just fix the camera not updating until the first mouse movement
    look(0, 0);
    firstMouse = true;
  }

  void move(Direction direction, float deltaTime) {
    float velocity = moveSpeed * deltaTime;

    switch (direction) {
    case Forward:
      position += velocity * frontFace;
      break;
    case Backward:
      position -= velocity * frontFace;
      break;
    case Left:
      position -= velocity * glm::normalize(glm::cross(frontFace, upVec));
      break;
    case Right:
      position += velocity * glm::normalize(glm::cross(frontFace, upVec));
      break;
    case Up:
      position.y += velocity;
      break;
    case Down:
      position.y -= velocity;
      break;
    }
  }

  void look(double xpos, double ypos) {
    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= lookSens;
    yoffset *= lookSens;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontFace = glm::normalize(direction);
  }

  void zoom(double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
      fov = 1.0f;
    else if (fov > 120.0f)
      fov = 120.0f;
  }

private:
  float lastX = 400;
  float lastY = 300;
  bool firstMouse = true;
};

#endif
