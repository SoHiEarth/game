#include "core/physics/physics.h"
#include <box2d/box2d.h>
#include "core/core.h"

void p::Update() {
  b2World_Step(Core::world.physics_world, 1.0f/60.0f, 6);
}

void p::ApplyForce(b2BodyId body, const glm::vec2& force) {
  b2Body_ApplyForceToCenter(body, b2Vec2(force.x, force.y), true);
}

void p::ApplyImpulse(b2BodyId body, const glm::vec2& impulse) {
  b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(impulse.x, impulse.y), true);
}

glm::vec2 p::GetPosition(b2BodyId body) {
  b2Vec2 pos = b2Body_GetPosition(body);
  return glm::vec2(pos.x, pos.y);
}

void p::SetPosition(b2BodyId body, const glm::vec2& position) {
  b2Body_SetTransform(body, b2Vec2(position.x, position.y), b2Body_GetRotation(body));
}

glm::vec2 p::GetVelocity(b2BodyId body) {
  b2Vec2 vel = b2Body_GetLinearVelocity(body);
  return glm::vec2(vel.x, vel.y);
}

void p::SetVelocity(b2BodyId body, const glm::vec2& velocity) {
  b2Body_SetLinearVelocity(body, b2Vec2(velocity.x, velocity.y));
}
