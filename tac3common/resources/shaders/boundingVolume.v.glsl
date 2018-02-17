#version 430

layout( location = 0 ) in vec3 position;
layout( location = 3 ) in vec3 normal;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 invTransWorld;

out vec3 wsNormal;
out vec4 wsPos; 

uniform mat4 unitToLocal;

void main()
{
  wsNormal = normalize( ( invTransWorld * vec4( normal, 0.0 ) ).xyz );
  wsPos = world * unitToLocal * vec4( position, 1.0 );
	gl_Position = proj * view * wsPos;
}
