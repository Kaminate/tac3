#version 430
uniform sampler2D cubeTextures[6];
in vec3 cubemapPoint;
out vec4 outColor;
vec4 MyTextureCube( vec3 r )
{
  int greatestDirIndex = 0;
  vec3 cubeDirs[] = vec3[]( // temp as a fuck

    vec3(-1, 0, 0), // left
    vec3( 1, 0, 0), // r
    vec3( 0, 0,-1), // bottom
    vec3( 0, 0, 1), // top
    vec3( 0,-1, 0), // back
    vec3( 0, 1, 0) // front
    );
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

void main()
{
  outColor = MyTextureCube(cubemapPoint);
}
