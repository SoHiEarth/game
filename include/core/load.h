#ifndef LOAD_H
#define LOAD_H

#include "core/world.h"
#include "core/player.h"
#include "core/npc.h"
#include "core/decoration.h"
#include "core/render/light.h"

namespace l {
  World LoadWorld(const char* filename);
  Camera LoadCamera(const char* filename, World& world);
  Player LoadPlayer(const char* filename, World& world);
  NPC LoadNPC(const char* filename, World& world);
  Decoration LoadDecoration(const char* filename, World& world);
  Light LoadLight(const char* filename, World& world);
}

#endif
