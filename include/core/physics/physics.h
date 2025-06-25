#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

struct b2BodyId;

namespace p {
  void Update();
  void ApplyForce(b2BodyId body, const glm::vec2& force);
  void ApplyImpulse(b2BodyId body, const glm::vec2& impulse);
  glm::vec2 GetPosition(b2BodyId body);
  void SetPosition(b2BodyId body, const glm::vec2& position);
  glm::vec2 GetVelocity(b2BodyId body);
  void SetVelocity(b2BodyId body, const glm::vec2& velocity);
}

#endif
