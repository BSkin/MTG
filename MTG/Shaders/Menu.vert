#version 150

uniform mat4 worldViewProj;
uniform float time;
uniform float mouseX;
uniform float mouseY;
uniform float width;
uniform float height;
uniform float enableLighting;
uniform float selectedCard;
uniform mat4 normalMatrix;

attribute vec3 vertex;
attribute vec2 uv;
attribute vec3 normal;

out float t;
out float lighting;
out float mX;
out float mY;
out float sX;
out float sY;
out float selected;
out vec2 uvVar;
out vec3 normalVar;

void main()
{
	t = time;
	mX = mouseX;
	mY = mouseY;
	sX = width;
	sY = height;
	lighting = enableLighting;
	selected = selectedCard;
	uvVar = uv;
	gl_Position = worldViewProj * vec4(vertex, 1.0);
	normalVar = vec3(normalMatrix * vec4(normal, 0.0f));
}