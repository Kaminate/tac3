#version 430

layout(location = 0) in vec3 position;
layout(location = 3) in vec3 normal;
layout(location = 0) uniform mat4 world;
// layout(location = 1) uniform mat4 view;
uniform mat4 view;
// layout(location = 2) uniform mat4 proj;
uniform mat4 proj;

out vec3 wsPos;
out vec3 wsNor;

void main()
{
  wsNor = normalize( ( world * vec4( normal, 0.0 ) ).xyz );
  // wsNor = normalize( ( view * world * vec4( normal, 0.0 ) ).xyz );
  vec4 wsPos4 = world * vec4( position, 1.0 );
  wsPos = wsPos4.xyz;
  gl_Position = proj * view * wsPos4;
}
