#version 430

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
// layout(location = 6) in vec3 tangent;
// layout(location = 7) in vec3 bitangent;
layout(location = 0) uniform mat4 world;
// layout(location = 1) uniform mat4 view;
uniform mat4 view;
// layout(location = 2) uniform mat4 proj;
uniform mat4 proj;
layout(location = 3) uniform mat4 invTransWorld;

out vec2 uvs;
out vec3 wsNormal;
// out vec3 msTangent;
// out vec3 msBitangent;


out vec4 wsPos; 
out vec3 msNormal;

void main(void)
{
  wsPos = world * vec4(position, 1.0);
	gl_Position = proj * view * wsPos;
	wsNormal = normalize((invTransWorld * vec4(normal, 0.0)).xyz);
  // msNormal = normal;

  // msTangent = tangent;
  // msBitangent = bitangent;
  uvs = uv;
}
