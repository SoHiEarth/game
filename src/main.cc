#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/core.h"
#include "core/render/render.h"
#include "core/load.h"
#include <fmt/core.h>

int main() {
  r::Init();
  Core::world = LoadWorld("testing/test.xml");

  Core::world.lights.push_back({
    {0.0f, 10.0f, 0.0f}, // Position
    {1.0f, 1.0f, 1.0f}, // Color
    1.0f // Intensity
  });

  while (Core::running) {
    if (glfwWindowShouldClose(Core::window)) {
      Core::running = false;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      Core::running = false;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_D) == GLFW_PRESS) {
      Core::world.player.transform.position.x += 0.1f;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_A) == GLFW_PRESS) {
      Core::world.player.transform.position.x -= 0.1f;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_W) == GLFW_PRESS) {
      Core::world.player.transform.position.y += 0.1f;
    }
    if (glfwGetKey(Core::window, GLFW_KEY_S) == GLFW_PRESS) {
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
    glfwPollEvents(); // Replace with i::
  }
  r::Quit();
  return 0;
}
