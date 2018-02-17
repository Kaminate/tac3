#version 430

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 invTransWorld;

out vec2 uvs;
out vec3 wsNormal;
out vec4 wsPos; 

void main(void)
{
  wsPos = world * vec4(position, 1.0);
	gl_Position = proj * view * wsPos;
	wsNormal = normalize((invTransWorld * vec4(normal, 0.0)).xyz);
  uvs = uv;
}
