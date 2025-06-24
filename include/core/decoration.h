#ifndef DECORATION_H
#define DECORATION_H

#include "core/transform.h"
#include <vector>

struct Decoration {
  private:
    unsigned int decoration_current_iteration = 0,
      decoration_frames_since_switch = 0;
  public:
    Transform transform;
    unsigned int GetTexture();
    unsigned int animation_frametime = 100; // How many frames until switching
    std::vector<unsigned int> textures;
    void Start();
    void Update();
    void Quit();
};

#endif
