#version 150

uniform sampler2D texS;

in vec2 uvVar;

out vec4 outColour;

void main()
{
	outColour = texture(texS, uvVar).rgba;
}