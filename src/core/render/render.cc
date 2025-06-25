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
#include <fmt/core.h>
#include <map>

struct Renderable {
  unsigned int texture_id;
  Transform transform;
};

std::vector<Renderable> renderables;
unsigned int texture_shader;
unsigned int texture_vao, texture_vbo, texture_ebo;
std::map<std::string, unsigned int> texture_cache;
unsigned int CreateProgram(const char* v_filename, const char* f_filename);

void WindowResizeCB(GLFWwindow* window, int width, int height) {
  Core::window_width = width;
  Core::window_height = height;
  glViewport(0, 0, width, height);
}

void r::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  Core::window_width = mode->width;
  Core::window_height = mode->height;
  Core::target_framerate = mode->refreshRate;
  Core::window = glfwCreateWindow(Core::window_width, Core::window_height, "Game", glfwGetPrimaryMonitor(), nullptr);
  glfwSetFramebufferSizeCallback(Core::window, WindowResizeCB);
  glfwMakeContextCurrent(Core::window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
  }
  // First load will need manual querying of window size
  int width, height;
  glfwGetFramebufferSize(Core::window, &width, &height);
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Disable VSync
  glfwSwapInterval(0);

  float vertices[] = {
    // positions        // texture coords
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
  };
  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };
  glGenVertexArrays(1, &texture_vao);
  glGenBuffers(1, &texture_vbo);
  glGenBuffers(1, &texture_ebo);
  glBindVertexArray(texture_vao);
  glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  texture_shader = CreateProgram("shaders/texture.vert", "shaders/texture.frag");
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
  texture_shader = CreateProgram("shaders/buffer.vert", "shaders/buffer.frag");
}

void r::DestroyShaders() {
  glDeleteProgram(texture_shader);
}

void r::Quit() {
  r::DestroyShaders();
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

void r::Render() {
  glUseProgram(texture_shader);
  glBindVertexArray(texture_vao);
  for (const auto& renderable : renderables) {
    glBindTexture(GL_TEXTURE_2D, renderable.texture_id);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(texture_shader, "tex"), 0);
    glm::mat4 model = renderable.transform.GetModelMatrix();
    glm::mat4 view = Core::world.camera.GetViewMatrix();
    glm::mat4 projection = Core::world.camera.GetProjectionMatrix();
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(glGetUniformLocation(texture_shader, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}

void r::EndFrame() {
  glfwSwapBuffers(Core::window);
  glfwPollEvents();
}

unsigned int r::LoadTexture(const char *filename) {
  if (texture_cache.find(filename) != texture_cache.end()) {
    return texture_cache[filename];
  }
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format = (nrChannels == 1) ? GL_RED : (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture: " << filename << std::endl;
  }
  stbi_image_free(data);
  texture_cache[filename] = textureID;
  return textureID;
}

