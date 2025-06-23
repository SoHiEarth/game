#ifndef NPC_H
#define NPC_H

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <string>
#include "core/transform.h"

struct NPC {
  std::string name;
  Transform transform;
  b2BodyId physics_body;
  unsigned int texture;
};

#endif
