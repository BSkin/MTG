#version 150

uniform sampler2D texS;

in vec2 uvVar;

out vec4 outColour;

void main()
{
	outColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);//texture(texS, uvVar).rgba;
}