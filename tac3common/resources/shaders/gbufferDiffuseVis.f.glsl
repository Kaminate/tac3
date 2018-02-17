#version 430

in vec2 uvs;
out vec4 outColor;
layout( location = 0 ) uniform sampler2D quadTexture;

void main()
{
  outColor = texture2D( quadTexture, uvs );
}

