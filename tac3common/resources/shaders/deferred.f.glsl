#version 430

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
in vec2 uvs;
in vec3 wsNormal;
out vec3 outTextures[ 3 ];

void main()
{
  outTextures[ 0 ] = texture2D( diffuseTexture, uvs ).xyz;
  outTextures[ 1 ] = wsNormal + 1.0 / 2.0;
  outTextures[ 2 ] = texture2D( specularTexture, uvs ).xyz;
}
