#include "core/decoration.h"



unsigned int Decoration::GetTexture() {
  return textures[decoration_current_iteration];
}

void Decoration::Start() {
  decoration_current_iteration = 0;
  decoration_frames_since_switch = 0;
}

void Decoration::Update() {
  decoration_frames_since_switch++;
  if (decoration_frames_since_switch >= animation_frametime) {
    decoration_current_iteration = (decoration_current_iteration < textures.size() - 1) ? decoration_current_iteration + 1 : 0;
    decoration_frames_since_switch = 0;
  }
}

void Decoration::Quit() {
  // Cleanup if necessary
  textures.clear();
}
