#version 430

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D specularTexture;

uniform vec3 wsLightCenter;
uniform vec3 lightDiffuse;

uniform int viewportX;
uniform int viewportY;
uniform int viewportW;
uniform int viewportH;

uniform float n;
uniform float f;

uniform mat4 invProj;
uniform mat4 invView;

uniform float lightRadius;

uniform vec3 wsCamPos;

in vec3 wsNor;
in vec3 wsPos;
out vec3 outColor;

float ToCameraZ( float depth01, float n, float f )
{
  float A = ( -n -f ) / ( f - n );
  float B = ( -2.0 * n * f ) / ( f - n );
  float zNDC = depth01 * 2.0 - 1.0;
  float zView = -B / ( zNDC + A );

  return zView; // NOTE: range is [ -n, -f ]
}

void main()
{
  vec2 uvs;
  uvs.x = ( gl_FragCoord.x - viewportX ) / viewportW;
  uvs.y = ( gl_FragCoord.y - viewportY ) / viewportH;

  vec3 diffuse = texture2D( diffuseTexture, uvs ).xyz;
  vec3 specular = texture2D( specularTexture, uvs ).xyz;

  float depth01 = texture2D( depthTexture, uvs ).x;
  float vsZ = ToCameraZ( depth01, n, f );

  vec4 ndcGeoPos;
  ndcGeoPos.x = uvs.x * 2.0 - 1.0;
  ndcGeoPos.y = uvs.y * 2.0 - 1.0;
  ndcGeoPos.z = depth01 * 2.0 - 1.0;
  ndcGeoPos.w = 1.0;

  vec4 clipGeoPos = ndcGeoPos * -vsZ;
  vec4 viewGeoPos = invProj * clipGeoPos;
  vec4 worldGeoPos = invView * viewGeoPos;

  vec3 wsGeoToLight = wsLightCenter - worldGeoPos.xyz;
  vec3 wsGeoToCamera = wsCamPos - worldGeoPos.xyz;
  vec3 l = normalize( wsGeoToLight );
  vec3 v = normalize( wsGeoToCamera );
  vec3 n = ( texture2D( normalTexture, uvs ).xyz * 2.0 ) - 1.0;
  vec3 h = normalize( l + v );


  float distSq = dot( wsGeoToLight, wsGeoToLight );
  float radiusSq = lightRadius * lightRadius;
  float attenuation = clamp(
    1.0 - ( distSq / radiusSq ), 
    0.0,
    1.0 );
  attenuation *= attenuation;

  float ndotl = max( dot( n, l ), 0.0 );
  float ndoth = max( dot( n, l ), 0.0 ); // approx dot( r, v )
  float intensity = 1;
  outColor = attenuation * (
    diffuse * lightDiffuse * intensity * ndotl + 

    // TEMP( spec is white, so we're using diffuse color instead )
    0.20001* diffuse * pow( ndoth, 8.0 ) * specular );



  // special light effects
  {
    float masterLightStrength = 0.26;

    float lightIntersectStrength = 0.5 * masterLightStrength;
    float lightRimStrength = 0.3001 * masterLightStrength;
    float lightFillStrength = 0.025 * masterLightStrength;


    //
    // draw where the light intersects geometry
    //
    float distLightToGeo = distance( wsLightCenter, worldGeoPos.xyz );
    // TODO: remove the if
    if(distLightToGeo > lightRadius - 0.3 &&
      distLightToGeo < lightRadius )
    {
      outColor += lightDiffuse * lightIntersectStrength * ndotl;
    }

    //
    // draw a ring around the edges of the sphere
    //
    vec3 wsCamToGeo = normalize( worldGeoPos.xyz - wsCamPos );
    float val = 1.0f - abs( dot( wsNor, -normalize( wsCamToGeo) ) );
    // TODO: remove the if
    if( val > 0.80 )
    {
      outColor += lightDiffuse * val * lightRimStrength;
    }

    // fill in the sphere with color
    outColor += lightDiffuse * lightFillStrength;
  }

}
