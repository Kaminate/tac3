#version 430

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

out vec2 uvs;

void main(void)
{
  gl_Position = vec4(position, 1.0);
  uvs = uv;
}
