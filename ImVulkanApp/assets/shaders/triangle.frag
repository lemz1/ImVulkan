#version 460 core

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 v_Color;

void main() 
{
	fragColor = vec4(v_Color, 1.0);
}
