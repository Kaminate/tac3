#version 430
// struct Light
// {
// vec3 wsPos; // padding
// vec3 wsDir; // padding
// vec3 diffuse; // padding
// vec3 ambient; // padding
// vec3 specular; // padding
// };

// layout(std140) uniform LightBlock
// {
//   Light lights[8];
//   int numPoints;
//   int numSpots;
//   int numDirs;
//   int iPoint;
//   int iSpot;
//   int iDir;
// };

uniform vec3 wsCamPos;
// uniform vec3 attenuationVec;
// uniform vec3 globalAmbient;
// uniform float innerSpotAngle;
// uniform float outerSpotAngle;
// uniform float spotFalloff;
// uniform float camNear;
// uniform float camFar;
// uniform float fogNearPercent;
// uniform float fogFarPercent;
// uniform vec3 fogColor;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform sampler2D cubeTextures[6];
// uniform sampler2D cube0;
// uniform sampler2D cube1;
// uniform sampler2D cube2;
// uniform sampler2D cube3;
// uniform sampler2D cube4;
// uniform sampler2D cube5;


// (Vector3(-1, 0, 0)); // left
// (Vector3( 1, 0, 0)); // r
// (Vector3( 0, 0,-1)); // bottom
// (Vector3( 0, 0, 1)); // top
// (Vector3( 0,-1, 0)); // back
// (Vector3( 0, 1, 0)); // front
uniform vec3 cubeDirs[6];
uniform vec3 cubeRights[6];
uniform vec3 cubeUps[6];

uniform bool useNormalMapping;
uniform float reflectionRefractionSlider;
// uniform bool useTangentBinormal;
// layout(location = 0) uniform mat4 world;



in vec2 uvs;
in vec4 wsPos;
in vec3 wsNormal;
// in vec3 msNormal;
// in vec3 msTangent;
// in vec3 msBitangent;
out vec4 outColor;
vec3 actualWsNormal;

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
  vec3 diffusecolor =  texture2D(diffuseTexture, uvs).xyz / 1.0;
  if(length(diffusecolor) == 0 )
  {
    diffusecolor = vec3(1,1,1);
  }
  return diffusecolor;
}

vec3 GetSpecular()
{
  vec3 specularColor = texture2D(specularTexture, uvs).xyz / 1.0;
  if(length(specularColor) == 0)
  {
    specularColor = vec3 (1,1,1);
  }
  return specularColor;
}

vec3 GetEmissive()
{
  return vec3(1,1,1) * 0.0;
}

vec4 MyTextureCube( vec3 r )
{
  int greatestDirIndex = 0;
  float greatestDot = dot(cubeDirs[0], r);
  for(int i = 1; i < 6; ++i)
  {
    float curDot = dot(cubeDirs[i], r);
    if(curDot > greatestDot)
    {
      greatestDirIndex = i;
      greatestDot = curDot;
    }
  }


  vec4 toReturn = vec4(0,0,0,1);
  switch(greatestDirIndex)
  {
    case 0: // left
    {
      vec3 intersectionPt = -r / r.x;

      vec2 intersectionUVs = vec2(
        (intersectionPt.y + 1.0) / 2.0,
        (intersectionPt.z + 1.0) / 2.0 );

      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
    case 1: // right
    {
      vec3 intersectionPt = r / r.x;

      vec2 intersectionUVs = vec2(
        (intersectionPt.y - 1.0) / -2.0,
        (intersectionPt.z + 1.0) / 2.0 );

      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
    case 2: // bottom
    {
      vec3 intersectionPt = -r / r.z;
      vec2 intersectionUVs = vec2(
        (intersectionPt.x + 1) / -2,
        (intersectionPt.y + 1) / -2);

      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
    case 3: // top
    {
      vec3 intersectionPt = r / r.z;
      vec2 intersectionUVs = vec2(
        (intersectionPt.x + 1) / 2,
        (intersectionPt.y + 1) / 2);
      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
    case 4: // back
    {
      vec3 intersectionPt = -r / r.y;
      vec2 intersectionUVs = vec2(
        (intersectionPt.x + 1) / -2,
        (intersectionPt.z + 1) / 2);
      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
    case 5: // front
    {
      vec3 intersectionPt = r / r.y;
      vec2 intersectionUVs = vec2(
        (intersectionPt.x + 1) / 2,
        (intersectionPt.z + 1) / 2);
      toReturn
        += texture2D( cubeTextures[ greatestDirIndex ], intersectionUVs );
      break;
    }
  }

  return toReturn;
}



float fresnel_step (vec3 I, vec3 N, vec3 f)
{
  return clamp(
    f.x + f.y * pow (1.0 + dot (I, N), f.z), 
    0.0, 
    1.0);
}

void main()
{
  if(useNormalMapping)
  {
    actualWsNormal = perturb_normal(wsNormal, wsPos.xyz, uvs);
  }
  else
  {
    actualWsNormal = wsNormal;
  }
  bool drawNormals = false;

  vec4 finalColor = vec4(0,0,0,1);
  vec3 toCam = normalize(wsCamPos - wsPos.xyz);

#define USING_GLSL_REFLECT 0
#define USING_GLSL_REFRACT 0


float eta = 1.0 / 1.33;
#if USING_GLSL_REFRACT == 1
  vec3 refractVec = refract(toCam, actualWsNormal, 1.0/1.33);
#else
  vec3 refractVec = refract(toCam, actualWsNormal, 1.0/1.33);

  float k 
    = 1.0 - eta 
    * eta 
    * (1.0 - dot(toCam, actualWsNormal) * dot(toCam, actualWsNormal));
  if (k < 0.0)
    refractVec = vec3(0,0,0);
  else
    refractVec 
      = eta * toCam 
      - (eta * dot(toCam, actualWsNormal) + sqrt(k)) * actualWsNormal;
#endif


#if USING_GLSL_REFLECT == 1
  vec3 reflectVec = reflect(-toCam, actualWsNormal);
#else
  vec3 reflectVec = -toCam + 2.0 * actualWsNormal * dot(toCam, actualWsNormal);
#endif

  float R = -0.30;
  vec3 refractVecR = refract(toCam, actualWsNormal, eta * (1.0 + R));
  vec3 refractVecG = refract(toCam, actualWsNormal, eta );
  vec3 refractVecB = refract(toCam, actualWsNormal, eta * (1.0 - R));


  vec4 refractColor = vec4(
    MyTextureCube(refractVecR).r,
    MyTextureCube(refractVecG).g,
    MyTextureCube(refractVecB).b, 1.0);
  // refractColor = MyTextureCube(refractVec);
  vec4 reflectColor = MyTextureCube(reflectVec);

  vec3 FR0 = vec3 (0.0, 1.0, 0.7);
  vec3 FR1 = vec3 (0.2, 1.0, 0.7);
  finalColor += mix(refractColor, reflectColor, 
    fresnel_step(-toCam, actualWsNormal, FR1));
    // reflectionRefractionSlider);
 
  finalColor.r += reflectionRefractionSlider / 1000.0;





  //////////////
  // good try //
  //////////////
  // mat3 world_to_plane = transpose ( mat3(
  //   cubeRights[ greatestDirIndex ],
  //   cubeUps[ greatestDirIndex ],
  //   -cubeDirs[ greatestDirIndex ] ) );
  // vec3 planespaceR = world_to_plane * r;
  // vec3 planespacePt = -planespaceR / planespaceR.z;
  // vec2 intersectionUVs = ( planespacePt.xy + 1.0 ) / 2.0;
  // finalColor.xyz += vec3(intersectionUVs.xy, 1);


  if(drawNormals)
  {
    finalColor.x += texture2D(normalTexture, uvs).x / 1000.0;
    finalColor /= 1000.0;
    finalColor += ToColor(actualWsNormal);
  }
  for(int i = 0; i < 6; ++i)
  {
    finalColor.xyz += texture2D(cubeTextures[i], vec2(0,0)).xyz / 1000.0;
  }
  for(int i = 0; i < 6; ++i)
  {
    finalColor.a += cubeRights[i].x / 1000.0;
    finalColor.a += cubeUps[i].x / 1000.0;
    finalColor.a += cubeDirs[i].x / 1000.0;
  }
  float ndotlololol = max( dot( actualWsNormal, vec3(0,-1,0) ), 0.0 );
  finalColor.xyz 
    += (GetDiffuse() * ndotlololol 
    + GetSpecular() * 0.20
    + GetEmissive()) 
    / 1000.0;

  // float distFromCamera = distance(wsPos.xyz, wsCamPos);
  // float fogNearDist = camNear + (camFar - camNear) * fogNearPercent;
  // float fogFarDist = camNear + (camFar - camNear) * fogFarPercent;
  // float fogPercent  = clamp(
  //   ( distFromCamera - fogNearDist ) 
  //   / (fogFarDist - fogNearDist), 0, 1);
  // finalColor.xyz += mix(finalc, fogColor, fogPercent);


  // finalColor /= 1000.0;
  // finalColor += mix(ToColor(reflectVec),
  // ToColor(refractVec), 0.3);

  outColor = finalColor;
}
