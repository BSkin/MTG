#version 150

uniform sampler2D texS;

in float t;
in float mX;
in float mY;
in float sX;
in float sY;
in float lighting;
in float selected;
in vec2 uvVar;
in vec3 normalVar;

out vec4 outColour;

void main()
{
	outColour = texture(texS, uvVar).rgba;
	
	if (selected > 0.5f && mX > -sX/2.0f && mX < sX/2.0f && mY > -sY/2.0f && mY < sY/2.0f) {
		float yThick = 30.0f;
		float pixelSection = floor(uvVar.y * sY / yThick);
		float cursorSection = floor((-mY+sY/2.0f) / yThick);
		if (pixelSection == cursorSection)
			outColour += vec4(0.25f, 0.25f, 0.25f, 0.0f);
		//outColour = vec4(0.855f, 0.647f, 0.125f, outColour.a);
	}
}