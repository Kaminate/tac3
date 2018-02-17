#version 430
uniform vec3 wsCamPos;
in vec4 wsPos;
in vec3 vColor;
in vec3 wsNormal;
out vec4 outColor;

void main()
{
  vec3 wsCamToPixel = normalize( wsPos.xyz - wsCamPos );
  float ndotl = max( dot( wsNormal, -wsCamToPixel ), 0 );
  // outColor = vec4( vec3( 1, 1, 1 ) * ndotl, 1.0 );
  outColor = vec4( vColor * ndotl, 1.0 );
  // outColor /= 1000.0;

  // outColor.xyz += vColor;
  // outColor.a += 1.0;
  
  // outColor.xyz += (wsNormal + 1.0) / 2.0;
  // outColor.a += 1.0;
}
