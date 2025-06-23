#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/core.h"
#include "core/render/render.h"
#include "core/load.h"

int main() {
  r::Init();
  Core::world = LoadWorld("testing/test.xml");

  while (Core::running) {
    if (glfwWindowShouldClose(Core::window)) {
      Core::running = false;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      Core::running = false;
    }
    r::BeginFrame();
    for (auto& npc : Core::world.npcs) {
      r::AddRenderable(npc.texture, npc.transform);
    }
    for (auto& decoration : Core::world.decorations) {
      r::AddRenderable(decoration.texture, decoration.transform);
    }
    r::AddRenderable(Core::world.player.texture, Core::world.player.transform);
    r::RenderGBuffer();
    r::RenderLighting();
    r::EndFrame();
    glfwPollEvents(); // Replace with i::
  }
  r::Quit();
  return 0;
}
