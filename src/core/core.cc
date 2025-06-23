#include "core/core.h"
#include <GLFW/glfw3.h>
#include "core/world.h"

GLFWwindow* Core::window = nullptr;
int Core::window_width = 800,
    Core::window_height = 600;
bool Core::running = true;
World Core::world;
