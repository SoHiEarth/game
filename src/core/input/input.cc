#include "core/input/input.h"
#include <GLFW/glfw3.h>
#include "core/core.h"

void i::PollEvents() {
  glfwPollEvents();
}

bool i::KeyPressed(int key) {
  return glfwGetKey(Core::window, key) == GLFW_PRESS;
}

bool i::KeyReleased(int key) {
  return glfwGetKey(Core::window, key) == GLFW_RELEASE;
}
