#ifndef GUIREGION_H
#define GUIREGION_H

#include "glm/glm.hpp"

class GUIRegion
{
public:
	GUIRegion(float l, float t, float r, float b, void (*f)()) { left = l; top = t; bot = b; right = r; function = f; }
	~GUIRegion() {}

	void callFunction() { if (function != NULL) function(); }
	float getLeft() { return left; }
	float getTop() { return top; }
	float getRight() { return right; }
	float getBot() { return bot; }
private:
	float left, top, right, bot;
	void (*function)();
};

#endif