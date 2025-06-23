#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;
struct Light {
  vec3 position;
  vec3 color;
  float intensity;
};

#define MAX_LIGHTS 64
uniform int u_num_lights;
uniform Light u_lights[MAX_LIGHTS];
uniform vec3 u_view_pos;

void main() {
  vec3 FragPos = texture(u_position, TexCoord).rgb;
  vec3 Normal = normalize(texture(u_normal, TexCoord).rgb * 2.0 - 1.0);
  vec3 Albedo = texture(u_albedo, TexCoord).rgb;
  vec3 lighting = vec3(0.0);
  for (int i = 0; i < u_num_lights; ++i) {
    Light light = u_lights[i];
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 viewDir = normalize(u_view_pos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    vec3 diffuse = diff * light.color * light.intensity;
    vec3 specular = 0.2 * spec * light.color * light.intensity;
    lighting += (diffuse + specular) * attenuation;
  }
  vec3 ambient = 0.1 * Albedo;
  vec3 color = ambient + lighting * Albedo;
  FragColor = vec4(color, 1.0);
}
