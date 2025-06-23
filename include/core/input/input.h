#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
namespace i {
  void PollEvents();
  bool KeyPressed(int key);
  bool KeyReleased(int key);
}

#endif
