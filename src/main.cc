#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include "core/core.h"
#include "core/render/render.h"
#include "core/input/input.h"
#include "core/physics/physics.h"
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
      p::ApplyImpulse(Core::world.player.physics_body, glm::vec2(0.1f, 0.0f));
    }
    if (i::KeyPressed(GLFW_KEY_A)) {
      p::ApplyImpulse(Core::world.player.physics_body, glm::vec2(-0.1f, 0.0f));
    }
    if (i::KeyPressed(GLFW_KEY_SPACE)) {
      p::ApplyImpulse(Core::world.player.physics_body, glm::vec2(0.0f, 0.1f));
    }

    glm::vec2 player_pos = p::GetPosition(Core::world.player.physics_body);
    Core::world.player.transform.position.x = player_pos.x;
    Core::world.player.transform.position.y = player_pos.y;
    // Clamp player velocity to prevent excessive speed
    glm::vec2 player_velocity = p::GetVelocity(Core::world.player.physics_body);
    player_velocity.x = std::clamp(player_velocity.x, -2.0f, 2.0f);
    player_velocity.y = std::clamp(player_velocity.y, -2.0f, 2.0f);
    p::SetVelocity(Core::world.player.physics_body, player_velocity);
    fmt::print("Player Position (vec3): ({}, {}, {})\n",
               Core::world.player.transform.position.x,
               Core::world.player.transform.position.y,
               Core::world.player.transform.position.z);

    Core::world.camera.transform.position = Core::world.player.transform.position;
    // set the camera a bit back in the z-axis
    Core::world.camera.transform.position.z += 5.0f;
    Core::world.Update();
    r::BeginFrame();
    Core::world.Render();
    r::Render();
    r::EndFrame();
    i::PollEvents();
    p::Update();
    auto frame_end = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);
    // Calculate sleep time to maintain target framerate
    auto target_frame_duration = std::chrono::milliseconds(1000 / Core::target_framerate);
    if (frame_duration < target_frame_duration) {
      std::this_thread::sleep_for(target_frame_duration - frame_duration);
    }
    fmt::print("Frame Duration: {} ms | FPS: {:.2f}\n", frame_duration.count(), 1000.0f / frame_duration.count());
  }
  r::Quit();
  return 0;
}
