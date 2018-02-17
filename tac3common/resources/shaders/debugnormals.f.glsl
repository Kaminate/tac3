#version 420

in vec3 lineColor;

// globals
out vec4 outColor;

void main()
{
  vec4 finalColor = vec4(0,0,0,1);
  finalColor.xyz += lineColor;
  outColor = finalColor;
}
