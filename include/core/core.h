#ifndef CORE_H
#define CORE_H

#include <GLFW/glfw3.h>
#include "core/world.h"

namespace Core {
  extern GLFWwindow* window;
  extern int window_width,
         window_height;
  extern bool running;
  extern World world;
  extern int target_framerate;
}

#endif
