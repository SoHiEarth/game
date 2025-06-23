#include "core/load.h"
#include "core/render/render.h"
#include <box2d/types.h>
#include <pugixml.hpp>
#include <box2d/box2d.h>

Transform LoadTransform(const pugi::xml_node& node) {
  Transform transform;
  transform.position.x = node.attribute("x").as_float();
  transform.position.y = node.attribute("y").as_float();
  transform.position.z = node.attribute("z").as_float();
  transform.scale.x = node.attribute("scale_x").as_float();
  transform.scale.y = node.attribute("scale_y").as_float();
  transform.rotation = node.attribute("rotation").as_float();
  return transform;
}

World l::LoadWorld(const char* filename) {
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

  world.gravity.x = world_node.attribute("gravity_x").as_float(0.0f);
  world.gravity.y = world_node.attribute("gravity_y").as_float(-9.81f);
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2(world.gravity.x, world.gravity.y);
  world.physics_world = b2CreateWorld(&world_def);
  world.ground_height = world_node.attribute("ground_height").as_int(10);
  b2BodyDef ground_body_def = b2DefaultBodyDef();
  ground_body_def.type = b2_staticBody;
  ground_body_def.position = b2Vec2(0.0f, static_cast<float>(world.ground_height - 1)/2);
  world.ground_body = b2CreateBody(world.physics_world, &ground_body_def);
  b2Polygon ground_box = b2MakeBox(5120.0f, 1.0f);
  b2ShapeDef ground_shape_def = b2DefaultShapeDef();
  ground_shape_def.material.friction = 0.5f;
  b2CreatePolygonShape(world.ground_body, &ground_shape_def, &ground_box);

  for (pugi::xml_node npc_node : world_node.children("npc")) {
    world.npcs.push_back(LoadNPC(npc_node.child_value(), world));
  }
  for (pugi::xml_node decoration_node : world_node.children("decoration")) {
    world.decorations.push_back(LoadDecoration(decoration_node.child_value(), world));
  }
  pugi::xml_node player_node = world_node.child("player");
  if (player_node) {
    world.player = LoadPlayer(player_node.child_value(), world);
  } else {
    fprintf(stderr, "No <player> node found in file '%s'\n", filename);
  }
  pugi::xml_node camera_node = world_node.child("camera");
  if (camera_node) {
    world.camera = LoadCamera(camera_node.child_value(), world);
  } else {
    fprintf(stderr, "No <camera> node found in file '%s'\n", filename);
  }
  return world;
}

Camera l::LoadCamera(const char* filename, World& world) {
  Camera camera;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    fprintf(stderr, "Failed to load camera file '%s': %s\n", filename, result.description());
    return camera;
  }
  pugi::xml_node camera_node = doc.child("camera");
  if (!camera_node) {
    fprintf(stderr, "No <camera> node found in file '%s'\n", filename);
    return camera;
  }
  camera.transform = LoadTransform(camera_node.child("transform"));
  return camera;
}

NPC l::LoadNPC(const char* filename, World& world) {
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

Decoration l::LoadDecoration(const char* filename, World& world) {
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

Player l::LoadPlayer(const char* filename, World& world) {
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
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_dynamicBody;
  body_def.fixedRotation = true;
  body_def.position = b2Vec2(player.transform.position.x/2, player.transform.position.y/2);
  player.physics_body = b2CreateBody(world.physics_world, &body_def);
  b2Polygon player_box = b2MakeBox(player.transform.scale.x/2, player.transform.scale.y/2);
  b2ShapeDef shape_def = b2DefaultShapeDef();
  shape_def.material.friction = 0.5f;
  b2CreatePolygonShape(player.physics_body, &shape_def, &player_box);
  return player;
}
