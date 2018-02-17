#version 430
struct Light
{
vec3 wsPos; // padding
vec3 wsDir; // padding
vec3 diffuse; // padding
vec3 ambient; // padding
vec3 specular; // padding
};

layout(std140) uniform LightBlock
{
  Light lights[8];
  int numPoints;
  int numSpots;
  int numDirs;
  int iPoint;
  int iSpot;
  int iDir;
};

uniform vec3 wsCamPos;
uniform vec3 attenuationVec;
uniform vec3 globalAmbient;
uniform float innerSpotAngle;
uniform float outerSpotAngle;
uniform float spotFalloff;
uniform float camNear;
uniform float camFar;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
layout(location = 3) uniform mat4 invTransWorld;

in vec2 uvs;
in vec4 wsPos;
in vec3 wsNormal;
out vec4 outColor;

vec4 ToColor(vec3 vec)
{
  return vec4(
    0.5 * vec.x + 0.5, 
    0.5 * vec.y + 0.5, 
    0.5 * vec.z + 0.5, 
    1);
}

mat3 transpose(mat3 m)
{
  return mat3(
    m[0][0], m[0][1], m[0][2],
    m[1][0], m[1][1], m[0][2],
    m[2][0], m[2][0], m[2][2]);
}
mat3 invert_mat3(mat3 m)
{
  float det = dot(cross(m[0], m[1]), m[2]);
  mat3 t = transpose(m);
  mat3 adj = mat3(
    cross(t[1], t[2]),
    cross(t[2], t[0]),
    cross(t[0], t[1]));
  return adj / det;
}

mat3 cotangent_frame(vec3 N, vec3 pos, vec2 texCoord)
{
  vec3 dp1  = dFdx(pos);
  vec3 dp2  = dFdy(pos);
  vec2 duv1 = dFdx(texCoord);
  vec2 duv2 = dFdy(texCoord);

  // solve the linear system
  vec3 dp2perp = cross( dp2, N );
  vec3 dp1perp = cross( N, dp1 );
  vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

  // construct a scale-invariant frame 
  float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
  return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal(vec3 N, vec3 pos, vec2 texCoord)
{
  vec4 map = texture(normalTexture, texCoord);
  map = map * 2 - 1;
  mat3 tbn = cotangent_frame(N, pos, texCoord);
  return normalize(tbn * map.xyz);
}


vec3 GetAmbient()
{
  return vec3(1,1,1) * 1;
}

vec3 GetDiffuse()
{
  return texture2D(diffuseTexture, uvs).xyz;
}

vec3 GetSpecular()
{
  return texture2D(specularTexture, uvs).xyz;
}

vec3 GetEmissive()
{
  return vec3(1,1,1) * 0.0;
}

void main()
{
  vec3 actualWsNormal = perturb_normal(wsNormal, wsPos.xyz, uvs);
  vec3 lightAccum = GetEmissive() + globalAmbient * GetAmbient();
  vec3 toCam = normalize(wsCamPos - wsPos.xyz);
  vec3 specularColor = GetSpecular();
  float shinyness = specularColor.r * 64.0;
  float specNormalization = (shinyness + 4.0) / 8.0;

  for(int i = 0; i < numPoints; ++i)
  {
    int iLight = iPoint + i;
    vec3 pointLightDir = normalize( wsPos.xyz- lights[iLight].wsPos);
    vec3 halfVec = normalize(toCam + -pointLightDir);
    float ndotl = max(dot(actualWsNormal, -pointLightDir), 0);
    float dist = distance(lights[iLight].wsPos, wsPos.xyz);
    float attenuation = 1.0f / (
      attenuationVec.x + 
      attenuationVec.y * dist +
      attenuationVec.z * dist * dist);
    float hdotn = max(dot(halfVec, actualWsNormal), 0);
    // specNormalization = 1;
    lightAccum 
      += lights[iLight].ambient * GetAmbient() 
      + ndotl * attenuation * lights[iLight].diffuse * GetDiffuse() 
      + lights[iLight].specular * pow(hdotn, shinyness) * specularColor 
      * specNormalization;
  }

  for(int i = 0; i < numSpots; ++i)
  {
    int iLight = iSpot + i;
    vec3 spotLightDir = normalize( wsPos.xyz - lights[iLight].wsPos );
    float ndotl = max(dot(actualWsNormal, -spotLightDir), 0);
    float dist = distance(lights[iLight].wsPos, wsPos.xyz);
    float attenuation = 1.0f / (
      attenuationVec.x + 
      attenuationVec.y * dist +
      attenuationVec.z * dist * dist);
    float cosInner = cos(innerSpotAngle); 
    float cosOuter = cos(outerSpotAngle); 
    float cosDir = dot(spotLightDir, lights[iLight].wsDir);
    float percentage = smoothstep(0,1, (cosOuter - cosDir) 
      / (cosOuter - cosInner));
    float spotlightEffect = pow(percentage, spotFalloff);
    vec3 halfVec = normalize(toCam + -spotLightDir);
    float hdotn = max(dot(halfVec, actualWsNormal), 0);
    lightAccum
      += lights[iLight].ambient * GetAmbient() 

      + ndotl 
      * attenuation 
      * lights[iLight].diffuse 
      * GetDiffuse() 
      * spotlightEffect

      + percentage 
      * lights[iLight].specular 
      * pow(hdotn, shinyness) 
      * specularColor
      * specNormalization;
  }

  for(int i = 0; i < numDirs; ++i)
  {
    int iLight = iDir + i;
    float ndotl = max(dot(actualWsNormal, -lights[iLight].wsDir),0);
    vec3 halfVec = normalize(toCam + -lights[iLight].wsDir);
    float hdotn = max(dot(halfVec, actualWsNormal), 0);
    lightAccum
      += lights[iLight].ambient * GetAmbient() 
      + ndotl * lights[iLight].diffuse * GetDiffuse() 
      + lights[iLight].specular * pow(hdotn, shinyness) 
      * specularColor * specNormalization;
  }

  outColor = vec4( 0, 0, 0, 1 );
  outColor.xyz += lightAccum;
}
