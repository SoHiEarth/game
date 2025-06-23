#include "core/load.h"
#include "core/render/render.h"
#include <pugixml.hpp>

Transform LoadTransform(const pugi::xml_node& node) {
  Transform transform;
  transform.position.x = node.attribute("x").as_float();
  transform.position.y = node.attribute("y").as_float();
  transform.scale.x = node.attribute("scale_x").as_float();
  transform.scale.y = node.attribute("scale_y").as_float();
  transform.rotation = node.attribute("rotation").as_float();
  return transform;
}

World LoadWorld(const char* filename) {
  World world;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    fprintf(stderr, "Failed to load world file '%s': %s\n", filename, result.description());
    return world;
  }
  pugi::xml_node world_node = doc.child("world");
  if (!world_node) {
    fprintf(stderr, "No <world> node found in file '%s'\n", filename);
    return world;
  }
  for (pugi::xml_node npc_node : world_node.children("npc")) {
    world.npcs.push_back(LoadNPC(npc_node.value()));
  }
  for (pugi::xml_node decoration_node : world_node.children("decoration")) {
    world.decorations.push_back(LoadDecoration(decoration_node.value()));
  }
  pugi::xml_node player_node = world_node.child("player");
  if (player_node) {
    world.player = LoadPlayer(player_node.value());
  } else {
    fprintf(stderr, "No <player> node found in file '%s'\n", filename);
  }
  return world;
}

NPC LoadNPC(const char* filename) {
  NPC npc;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    fprintf(stderr, "Failed to load NPC file '%s': %s\n", filename, result.description());
    return npc;
  }
  pugi::xml_node npc_node = doc.child("npc");
  if (!npc_node) {
    fprintf(stderr, "No <npc> node found in file '%s'\n", filename);
    return npc;
  }
  npc.name = npc_node.attribute("name").as_string();
  npc.transform = LoadTransform(npc_node.child("transform"));
  npc.texture = r::LoadTexture(npc_node.attribute("texture").as_string());
  return npc;
}

Decoration LoadDecoration(const char* filename) {
  Decoration decoration;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    fprintf(stderr, "Failed to load decoration file '%s': %s\n", filename, result.description());
    return decoration;
  }
  pugi::xml_node decoration_node = doc.child("decoration");
  if (!decoration_node) {
    fprintf(stderr, "No <decoration> node found in file '%s'\n", filename);
    return decoration;
  }
  decoration.transform = LoadTransform(decoration_node.child("transform"));
  decoration.texture = r::LoadTexture(decoration_node.attribute("texture").as_string());
  return decoration;
}

Player LoadPlayer(const char* filename) {
  Player player;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    fprintf(stderr, "Failed to load player file '%s': %s\n", filename, result.description());
    return player;
  }
  pugi::xml_node player_node = doc.child("player");
  if (!player_node) {
    fprintf(stderr, "No <player> node found in file '%s'\n", filename);
    return player;
  }
  player.transform = LoadTransform(player_node.child("transform"));
  player.texture = r::LoadTexture(player_node.attribute("texture").as_string());
  return player;
}
