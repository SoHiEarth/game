#ifndef CAMERA_H
#define CAMERA_H

#include "core/transform.h"

namespace Core {
  extern int window_width;
  extern int window_height;
}

struct Camera {
  Transform transform;
  glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4 GetProjectionMatrix() const {
    return glm::perspective(glm::radians(45.0f), static_cast<float>(Core::window_width) / Core::window_height, 0.1f, 100.0f);
  }
  glm::mat4 GetViewMatrix() const {
    glm::vec3 camera_position = transform.position,
      camera_target = camera_position + forward;
    return glm::lookAt(camera_position, camera_target, up);
  }
};

#endif
