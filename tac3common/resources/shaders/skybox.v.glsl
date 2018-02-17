#version 430

layout(location = 0) in vec3 position;
uniform mat4 view;
uniform mat4 proj;
out vec3 cubemapPoint;
void main(void)
{
  //                                         0 (instead of 1) 
  //                                           (prolly works)
  gl_Position = proj * view * vec4(position, 0);
  gl_Position.z = gl_Position.w; // far plane
  cubemapPoint = position;
}
