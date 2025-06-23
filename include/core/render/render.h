#ifndef RENDER_H
#define RENDER_H

#include "core/transform.h"

namespace r {
  void Init();
  void InitShaders();
  void DestroyShaders();
  void Quit();

  void BeginFrame();
  void AddRenderable(const unsigned int id, const Transform& transform);
  void Render();
  void EndFrame();

  unsigned int LoadTexture(const char* filename);
}

#endif
