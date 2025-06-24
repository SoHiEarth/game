#include "core/npc.h"

unsigned int NPC::GetTexture() {
  return textures[npc_current_iteration];
}

void NPC::Start() {
  npc_current_iteration = 0;
  npc_frames_since_switch = 0;
}

void NPC::Update() {
  npc_frames_since_switch++;
  if (npc_frames_since_switch >= animation_frametime) {
    npc_current_iteration = (npc_current_iteration < textures.size() - 1) ? npc_current_iteration + 1 : 0;
    npc_frames_since_switch = 0;
  }
}

void NPC::Quit() {
}
