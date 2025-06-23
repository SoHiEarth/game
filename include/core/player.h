#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include "core/transform.h"
#include <box2d/box2d.h>

struct Player {
  Transform transform;
  b2BodyId physics_body;
  unsigned int texture;
};

#endif
