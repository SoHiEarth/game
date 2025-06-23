#ifndef WORLD_H
#define WORLD_H

#include <box2d/box2d.h>
#include <vector>
#include "core/npc.h"
#include "core/player.h"
#include "core/decoration.h"
#include "core/render/light.h"
#include "core/camera.h"

struct World {
  std::vector<NPC> npcs;
  std::vector<Decoration> decorations;
  std::vector<Light> lights;
  Player player;
  Camera camera;
  b2WorldId physics_world;
  int ground_height;
  b2BodyId ground_body;
};

#endif
