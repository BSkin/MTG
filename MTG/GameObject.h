#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	

#include "AssetManager.h"
#include "Utilities.h"

class GameObject
{
public:
	GameObject();
	GameObject(string, string, string); //ok for now
	GameObject(string, string, string, glm::vec3 position, glm::vec3 direction, glm::vec3 up); //ok for now
	GameObject(string, string, string, glm::vec3 position, glm::vec3 direction, glm::vec3 up, glm::vec3 scale); //ok for now
	virtual ~GameObject();

	bool isAlive() { return alive; }
	void setAlive(bool x) { alive = x; }
	glm::vec3 getPosition() { return position; }
	glm::vec3 getDirection() { return direction; }
	glm::vec3 getUp() { return up; }
	glm::vec3 getScale() { return scale; }
	virtual Texture * getTexture() { return texture; }
	string getTexturePath() { return texturePath; }
	void changeTexture(string TexturePath) { texture = NULL; texturePath = TexturePath; }
	void changeTexture(Texture * t) { if (t != NULL) texture = t; }
	void setPosition(glm::vec3 x) { position = x; }
	void setPosition(float x, float y, float z) { position = glm::vec3(x, y, z); }
	void setDirection(glm::vec3 x) { direction = x; }
	void setDirection(float x, float y, float z) { direction = glm::vec3(x, y, z); }
	void setUp(glm::vec3 x) { up = x; }
	void setUp(float x, float y, float z) { up = glm::vec3(x, y, z); }
	void setScale(glm::vec3 x) { scale = x; }
	void setScale(float x, float y, float z) { scale = glm::vec3(x, y, z); }
	void setSelected(bool b = true) { selected = b; }

	virtual void updateWorldMatrix();
	virtual int update(long elapsedTime);
	virtual int render(long totalElapsedTime);
	static void setStatics(bool * ignore, glm::mat4 * view, glm::mat4 * proj, list<GameObject *> * goList, AssetManager * ass) 
		{ ignoreShader = ignore; viewMatrix = view; projMatrix = proj; gameObjectList = goList; assetManager = ass; }
protected:
	string modelPath;
	string texturePath;
	string shaderPath;

	Model * model;
	Texture * texture;
	Shader * shader;

	bool lighting;
	bool selected;

	glm::mat4 worldMatrix, normalMatrix;
	glm::vec3 position, direction, up, scale;
	bool alive;

	void init();

	static bool * ignoreShader;
	static glm::mat4 * viewMatrix;
	static glm::mat4 * projMatrix;
	static list<GameObject *> * gameObjectList;
	static AssetManager * assetManager;
};

#endif