#ifndef LOAD_H
#define LOAD_H

#include "core/world.h"
#include "core/player.h"
#include "core/npc.h"
#include "core/decoration.h"
#include "core/render/light.h"

World LoadWorld(const char* filename);
Player LoadPlayer(const char* filename);
NPC LoadNPC(const char* filename);
Decoration LoadDecoration(const char* filename);
Light LoadLight(const char* filename);

#endif
