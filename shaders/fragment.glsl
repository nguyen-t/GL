#version 330 core

uniform vec3 triangleColor;
out vec4 outColor;

void main()
{
  outColor = vec4(triangleColor, 1.0);
}
