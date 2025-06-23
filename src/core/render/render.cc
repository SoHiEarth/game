#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/core.h"
#include "core/render/render.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Renderable {
  unsigned int texture_id;
  Transform transform;
};

std::vector<Renderable> renderables;

void WindowResizeCB(GLFWwindow* window, int width, int height) {
  Core::window_width = width;
  Core::window_height = height;
  glViewport(0, 0, width, height);
  r::DestroyGBuffer();
  r::InitGBuffer();
}

unsigned int r::g_buffer,
             r::g_position,
             r::g_normal,
             r::g_albedo,
             r::g_depth;
unsigned int r::g_buffer_shader,
             r::g_lighting_shader;
unsigned int r::quad_vao, r::quad_vbo;

void r::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  Core::window_width = mode->width;
  Core::window_height = mode->height;
  Core::window = glfwCreateWindow(Core::window_width, Core::window_height, "Game", glfwGetPrimaryMonitor(), nullptr);
  glfwSetFramebufferSizeCallback(Core::window, WindowResizeCB);
  glfwMakeContextCurrent(Core::window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return;
  }
  glViewport(0, 0, Core::window_width, Core::window_height);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  InitGBuffer();
}

void r::InitGBuffer() {
  glGenFramebuffers(1, &g_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
  glGenTextures(1, &g_position);
  glBindTexture(GL_TEXTURE_2D, g_position);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Core::window_width, Core::window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, g_position, 0);
  glGenTextures(1, &g_normal);
  glBindTexture(GL_TEXTURE_2D, g_normal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Core::window_width, Core::window_height,
      0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
      GL_TEXTURE_2D, g_normal, 0);
  glGenTextures(1, &g_albedo);
  glBindTexture(GL_TEXTURE_2D, g_albedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Core::window_width, Core::window_height,
      0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
      GL_TEXTURE_2D, g_albedo, 0);
  unsigned int attachments[3] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2
  };
  glDrawBuffers(3, attachments);
  glGenRenderbuffers(1, &g_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, g_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Core::window_width, Core::window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete\n";
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void r::DestroyGBuffer() {
  glDeleteFramebuffers(1, &g_buffer);
  glDeleteTextures(1, &g_position);
  glDeleteTextures(1, &g_normal);
  glDeleteTextures(1, &g_albedo);
  glDeleteRenderbuffers(1, &g_depth);
}

unsigned int CompileShader(const char* filename, GLenum type) {
  std::ifstream shaderFile(filename);
  if (!shaderFile.is_open()) {
    std::cerr << "Failed to open shader file: " << filename << std::endl;
    return 0;
  }
  std::stringstream shaderStream;
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();
  std::string shaderCode = shaderStream.str();
  const char* shaderSource = shaderCode.c_str();
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Shader compilation error in " << filename << ": " << infoLog << std::endl;
  }
  return shader;
}

unsigned int CreateProgram(const char* v_filename, const char* f_filename) {
  unsigned int vertexShader = CompileShader(v_filename, GL_VERTEX_SHADER);
  unsigned int fragmentShader = CompileShader(f_filename, GL_FRAGMENT_SHADER);
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cerr << "Program linking error: " << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return program;
}

void r::InitShaders() {
  g_buffer_shader = CreateProgram("shaders/buffer.vert", "shaders/buffer.frag");
  g_lighting_shader = CreateProgram("shaders/lighting.vert", "shaders/lighting.frag");
  if (g_buffer_shader == 0 || g_lighting_shader == 0) {
    std::cerr << "Failed to initialize shaders." << std::endl;
    return;
  }
}

void r::SetupGeometry() {
  float quad_verts[] = {
    // positions   // texture coords
    -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.0f, 1.0f
  };
  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &quad_vbo);
  glBindVertexArray(quad_vao);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);
}

void r::Quit() {
  r::DestroyGBuffer();
  glfwDestroyWindow(Core::window);
  glfwTerminate();
}

void r::BeginFrame() {
  renderables.clear();
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r::AddRenderable(const unsigned int id, const Transform& transform) {
  renderables.push_back({id, transform});
}

void r::RenderGBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(g_buffer_shader);
  glm::mat4 projection = Core::world.camera.GetProjectionMatrix();
  glm::mat4 view = Core::world.camera.GetViewMatrix();
  glUniformMatrix4fv(glGetUniformLocation(g_buffer_shader, "u_projection"), 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(glGetUniformLocation(g_buffer_shader, "u_view"), 1, GL_FALSE, glm::value_ptr(view));
  glBindVertexArray(quad_vao);
  for (const auto& renderable : renderables) {
    glm::mat4 model = renderable.transform.GetModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(g_buffer_shader, "u_model"), 1, GL_FALSE, glm::value_ptr(model));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderable.texture_id);
    glUniform1i(glGetUniformLocation(g_buffer_shader, "u_texture"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderQuad() {
  float vertices[] = {
    // positions   // texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  static unsigned int quad_vao = 0, quad_vbo = 0;
  if (quad_vao == 0) {
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  }
  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void r::RenderLighting() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(g_lighting_shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_position);
  glUniform1i(glGetUniformLocation(g_lighting_shader, "u_position"), 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_normal);
  glUniform1i(glGetUniformLocation(g_lighting_shader, "u_normal"), 1);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_albedo);
  glUniform1i(glGetUniformLocation(g_lighting_shader, "u_albedo"), 2);
  glUniform1i(glGetUniformLocation(g_lighting_shader, "u_num_lights"), Core::world.lights.size());
  for (size_t i = 0; i < Core::world.lights.size(); ++i) {
    const auto& light = Core::world.lights[i];
    std::string prefix = "u_lights[" + std::to_string(i) + "].";
    glUniform3fv(glGetUniformLocation(g_lighting_shader, (prefix + "position").c_str()), 1, &light.position[0]);
    glUniform3fv(glGetUniformLocation(g_lighting_shader, (prefix + "color").c_str()), 1, &light.color[0]);
    glUniform1f(glGetUniformLocation(g_lighting_shader, (prefix + "intensity").c_str()), light.intensity);
  }
  RenderQuad();
}

void r::EndFrame() {
  glfwSwapBuffers(Core::window);
  glfwPollEvents();
}

unsigned int r::LoadTexture(const char *filename) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 1) ? GL_RED : (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}
