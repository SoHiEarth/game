#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoord;
in vec3 WorldPos;

uniform sampler2D u_texture;

void main() {
    gPosition = WorldPos;
    gNormal = vec3(0.0, 0.0, 1.0); // 2D sprites face forward
    gAlbedo = texture(u_texture, TexCoord);
}
