#version 150

uniform mat4 worldViewProj;

attribute vec3 vertex;
attribute vec2 uv;

out vec2 uvVar;

void main()
{
	uvVar = uv;
	gl_Position = worldViewProj * vec4(vertex, 1.0);
}