#ifndef RENDER_H
#define RENDER_H

#include "core/transform.h"

namespace r {
  extern unsigned int g_buffer,
         g_position,
         g_normal,
         g_albedo,
         g_depth;
  extern unsigned int g_buffer_shader,
         g_lighting_shader;
  extern unsigned int quad_vao, quad_vbo;

  void Init();
  void InitGBuffer();
  void DestroyGBuffer(); // Used when resizing, quitting
  void InitShaders();
  void DestroyShaders();
  void SetupGeometry();
  void Quit();

  void BeginFrame();
  void AddRenderable(const unsigned int id, const Transform& transform);
  void RenderGBuffer();
  void RenderLighting();
  void EndFrame();

  unsigned int LoadTexture(const char* filename);
  void RenderTexture(unsigned int textureID, float x, float y, float width, float height);
}

#endif
