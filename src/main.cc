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
    // Get frame start time
    auto frame_start = std::chrono::high_resolution_clock::now();
    if (glfwWindowShouldClose(Core::window)) {
      Core::running = false;
    }
    if (i::KeyPressed(GLFW_KEY_ESCAPE)) {
      Core::running = false;
    }
    if (i::KeyPressed(GLFW_KEY_D)) {
      b2Body_ApplyLinearImpulseToCenter(Core::world.player.physics_body, b2Vec2(0.1f, 0.0f), true);
    }
    if (i::KeyPressed(GLFW_KEY_A)) {
      b2Body_ApplyLinearImpulseToCenter(Core::world.player.physics_body, b2Vec2(-0.1f, 0.0f), true);
    }
    if (i::KeyPressed(GLFW_KEY_SPACE)) {
      b2Body_ApplyLinearImpulseToCenter(Core::world.player.physics_body, b2Vec2(0.0f, 0.5f), true);
    }

    b2Vec2 player_pos = b2Body_GetPosition(Core::world.player.physics_body);
    Core::world.player.transform.position.x = player_pos.x;
    Core::world.player.transform.position.y = player_pos.y;
    // Clamp player velocity to prevent excessive speed
    b2Vec2 player_velocity = b2Body_GetLinearVelocity(Core::world.player.physics_body);
    player_velocity.x = std::clamp(player_velocity.x, -2.0f, 2.0f);
    player_velocity.y = std::clamp(player_velocity.y, -2.0f, 2.0f);
    b2Body_SetLinearVelocity(Core::world.player.physics_body, player_velocity);
    fmt::print("Player Position (vec3): ({}, {}, {})\n",
               Core::world.player.transform.position.x,
               Core::world.player.transform.position.y,
               Core::world.player.transform.position.z);

    Core::world.camera.transform.position = Core::world.player.transform.position;
    // set the camera a bit back in the z-axis
    Core::world.camera.transform.position.z += 5.0f;
    r::BeginFrame();
    for (auto& npc : Core::world.npcs) {
      npc.Update();
      r::AddRenderable(npc.GetTexture(), npc.transform);
    }
    for (auto& decoration : Core::world.decorations) {
      decoration.Update();
      r::AddRenderable(decoration.GetTexture(), decoration.transform);
    }
    r::AddRenderable(Core::world.player.texture, Core::world.player.transform);
    r::Render();
    r::EndFrame();
    i::PollEvents();
    b2World_Step(Core::world.physics_world, 1.0f / 60.0f, 4);
    auto frame_end = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);
    // Calculate sleep time to maintain target framerate
    auto target_frame_duration = std::chrono::milliseconds(1000 / Core::target_framerate);
    if (frame_duration < target_frame_duration) {
      std::this_thread::sleep_for(target_frame_duration - frame_duration);
    }
  }
  r::Quit();
  return 0;
}
