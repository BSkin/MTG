#ifndef GUIITEM_H
#define GUIITEM_H

#include "GameObject.h"
#include "Camera.h"
#include "InputManager.h"
#include "GUIRegion.h"

#define yWidth 30.0f

class GUIItem : public GameObject
{
public:
	GUIItem(string texturePath, string shaderPath);
	virtual ~GUIItem();
	static void setStatics(Camera * c, InputManager * i, int * x, int * y, list<GUIItem*> * l) 
		{ camera = c; inputManager = i; screenWidth = x; screenHeight = y; GUIList = l; }

	void setScreenPosition(glm::vec2 x);
	void setScreenPosition(float x, float y) { setScreenPosition(glm::vec2(x,y)); }
	void setDimensions(glm::vec2 x) { dimensions = x; }
	void setDimensions(float x, float y) { dimensions = glm::vec2(x, y); }
	void addRegion(float left, float top, float right, float bot, void (*function)());

	virtual int updateGUI(long elapsedTime, int i);
	virtual int update(long elapsedTime);
	virtual int render(long totalElapsedTime);
protected:
	static Camera * camera;
	static InputManager * inputManager;
	static int * screenWidth, * screenHeight;
	static list<GUIItem*> * GUIList;

	list<GUIRegion*> regionList;
	glm::vec2 screenPos;
	glm::vec2 dimensions;
};

#endif