#version 430
uniform vec3 wsCamPos;
uniform vec4 color;
in vec4 wsPos;
in vec3 wsNormal;
out vec4 outColor;


void main()
{
  vec3 wsCamToPixel = normalize( wsPos.xyz - wsCamPos );
  float ndotl = max( dot( wsNormal, -wsCamToPixel ), 0 );
  outColor.xyz = ndotl * color.xyz;
  outColor.xyz /= 1000.0;
  outColor.xyz += color.xyz;
  // outColor.xyz += vec3( 1, 0, 0 );
  outColor.a = color.a;
  // outColor.a = 1.0;
}
