#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 TexCoord;
out vec3 WorldPos;

void main() {
    vec4 worldPos = u_model * vec4(aPos, 0.0, 1.0);
    WorldPos = worldPos.xyz;
    TexCoord = aTexCoord;
    gl_Position = u_projection * u_view * worldPos;
}
