#ifndef NPC_H
#define NPC_H

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <string>
#include <vector>
#include "core/transform.h"

struct NPC {
  private:
  unsigned int npc_current_iteration = 0,
    npc_frames_since_switch = 0;
  public:
    std::string name;
    Transform transform;
    b2BodyId physics_body;
    unsigned int GetTexture();
    unsigned int animation_frametime = 100; // How many frames until switching
    std::vector<unsigned int> textures; // Animation frames
    void Start();
    void Update();
    void Quit();
};

#endif
