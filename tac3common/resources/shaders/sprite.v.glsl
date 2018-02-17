#version 430

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
out vec2 uvs;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;


void main(void)
{
  gl_Position = proj * view * world * vec4(position, 1.0);
  uvs = uv;
}
