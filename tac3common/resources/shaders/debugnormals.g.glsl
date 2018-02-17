#version 430

layout(points) in;
layout(line_strip, max_vertices = 6) out;

layout(location = 3) uniform mat4 invTransWorld;
layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 proj;
layout(location = 6) uniform float arrowLen;


in vec4 wsPos[];
in vec3 wsNormal[];
in vec3 msTangent[];
in vec3 msBitangent[];
out vec3 lineColor;


void AddLine(vec3 begin, vec3 end, vec3 color)
{
  lineColor = color;

  gl_Position = proj * view * vec4(begin, 1.0);
  EmitVertex();

  gl_Position = proj * view * vec4(end, 1.0);
  EmitVertex();
}

void main()
{
  vec3 wsTangent 
    = normalize((invTransWorld * vec4(msTangent[0], 0.0)).xyz);
  vec3 wsBitangent 
    = normalize((invTransWorld * vec4(msBitangent[0], 0.0)).xyz);

  vec3 wsNormalPoint = wsPos[0].xyz + wsNormal[0] * arrowLen;
  vec3 wsTangentPoint = wsPos[0].xyz + wsTangent * arrowLen;
  vec3 wsBitangentPoint = wsPos[0].xyz + wsBitangent * arrowLen;

  float colorScale = 0.9;
  vec3 tangentColor   = vec3(colorScale,0,0);
  vec3 bitangentColor = vec3(0,colorScale,0);
  vec3 normalColor    = vec3(0,0,colorScale);

  AddLine(wsPos[0].xyz, wsTangentPoint, tangentColor);
  AddLine(wsPos[0].xyz, wsBitangentPoint, bitangentColor);
  AddLine(wsPos[0].xyz, wsNormalPoint, normalColor);

  EndPrimitive();
}
