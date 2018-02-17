#version 430

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 color;
layout( location = 3 ) in vec3 normal;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 invTransWorld;

out vec3 wsNormal;
out vec3 vColor;
out vec4 wsPos; 

void main()
{
  wsNormal = normalize( ( invTransWorld * vec4( normal, 0.0 ) ).xyz );
  wsPos = world * vec4( position, 1.0 );
	gl_Position = proj * view * wsPos;
  vColor = color;
}
