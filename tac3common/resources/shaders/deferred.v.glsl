#version 430

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;

layout(location = 0) uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

layout(location = 3) uniform mat4 invTransWorld;

out vec2 uvs;
out vec3 wsNormal;

void main()
{
  vec4 wsPos4 = world * vec4( position, 1.0 );
	wsNormal = normalize( ( invTransWorld * vec4( normal, 0.0 ) ).xyz );
  uvs = uv;
  gl_Position = proj * view * wsPos4;
}
