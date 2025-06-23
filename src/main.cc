#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include "core/core.h"
#include "core/render/render.h"
#include "core/input/input.h"
#include "core/load.h"
#include <fmt/core.h>
#include <thread>
#include <chrono>

int main() {
  r::Init();
  Core::world = l::LoadWorld("testing/test.xml");
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
    
    b2Vec2 player_pos = b2Body_GetPosition(Core::world.player.physics_body);
    Core::world.player.transform.position.x = player_pos.x;
    Core::world.player.transform.position.y = player_pos.y;

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
    b2World_Step(Core::world.physics_world, 1.0f / 60.0f, 4);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
  }
  r::Quit();
  return 0;
}
