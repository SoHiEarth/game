#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/core.h"
#include "core/render/render.h"
#include "core/input/input.h"
#include "core/load.h"
#include <fmt/core.h>

int main() {
  r::Init();
  Core::world = LoadWorld("testing/test.xml");
  while (Core::running) {
    if (glfwWindowShouldClose(Core::window)) {
      Core::running = false;
    }
    if (i::KeyPressed(GLFW_KEY_ESCAPE)) {
      Core::running = false;
    }
    if (i::KeyPressed(GLFW_KEY_D)) {
      Core::world.player.transform.position.x += 0.1f;
    }
    if (i::KeyPressed(GLFW_KEY_A)) {
      Core::world.player.transform.position.x -= 0.1f;
    }
    if (i::KeyPressed(GLFW_KEY_W)) {
      Core::world.player.transform.position.y += 0.1f;
    }
    if (i::KeyPressed(GLFW_KEY_S)) {
      Core::world.player.transform.position.y -= 0.1f;
    }
    Core::world.camera.transform.position = Core::world.player.transform.position;
    // set the camera a bit back in the z-axis
    Core::world.camera.transform.position.z += 5.0f;
    fmt::print("Camera Position: ({}, {}, {})\n",
           Core::world.camera.transform.position.x,
           Core::world.camera.transform.position.y,
           Core::world.camera.transform.position.z);
    r::BeginFrame();
    for (auto& npc : Core::world.npcs) {
      r::AddRenderable(npc.texture, npc.transform);
    }
    for (auto& decoration : Core::world.decorations) {
      r::AddRenderable(decoration.texture, decoration.transform);
    }
    r::AddRenderable(Core::world.player.texture, Core::world.player.transform);
    r::Render();
    r::EndFrame();
    i::PollEvents();
  }
  r::Quit();
  return 0;
}
