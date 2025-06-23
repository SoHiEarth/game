#ifndef CAMERA_H
#define CAMERA_H

#include "core/transform.h"

struct Camera {
  Transform transform;
  float zoom = 1.0f;
  float aspect_ratio = 1.0f;
  glm::mat4 GetViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(-transform.position));
  }
  glm::mat4 GetProjectionMatrix() const {
    float height = 10.0f / zoom;
    float width = height * aspect_ratio;
    return glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, 0.1f, 100.0f);
  }
};

#endif
