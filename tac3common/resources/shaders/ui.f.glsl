#version 420

uniform vec3 diffuse;

// globals
in vec4 wsPos;
in vec3 wsNormal;
out vec4 outColor;

void main()
{
  vec4 finalColor = vec4(0,0,0,1);
  finalColor.xyz += diffuse;
  finalColor.r += wsNormal.x/1000.0;
  outColor = finalColor;
}
