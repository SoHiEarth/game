#include "core/world.h"
#include "core/render/render.h"

void World::Start() {
  for (auto& npc : npcs) {
    npc.Start();
  }
  for (auto& decoration : decorations) {
    decoration.Start();
  }
}

void World::Update() {
  for (auto& npc : npcs) {
    npc.Update();
  }
  for (auto& decoration : decorations) {
    decoration.Update();
  }
}

void World::Render() {
  for (auto& npc : npcs) {
    r::AddRenderable(npc.GetTexture(), npc.transform);
  }
  for (auto& decoration : decorations) {
    r::AddRenderable(decoration.GetTexture(), decoration.transform);
  }
  r::AddRenderable(player.texture, player.transform);
}

void World::Quit() {
  for (auto& npc : npcs) {
    npc.Quit();
  }
  for (auto& decoration : decorations) {
    decoration.Quit();
  }
}
