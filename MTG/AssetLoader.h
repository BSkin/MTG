#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Utilities.h"
#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"

#include <windows.h>
#include <unordered_map>
#include <thread>
#include <string>
#include <list>
using std::string;
using std::list;
using std::unordered_map;
using std::make_pair;

#define modelMap unordered_map<string, Model *>
#define textureMap unordered_map<string, Texture *>
#define shaderMap unordered_map<string, Shader *>

struct SDL_DATA { HDC * window; HGLRC * loadingContext; HGLRC * primaryContext; }; 

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();
private:
	friend class AssetManager;

	void start();
	void loadModel(string path);
	void loadTexture(string path);
	void loadShader(string path);

	list<string> * objectsToLoad;
	modelMap	* models;
	textureMap	* textures;
	shaderMap	* shaders;

	HANDLE * objectsMutex;
	HANDLE * modelsMutex;
	HANDLE * texturesMutex;
	HANDLE * shadersMutex;

	static HGLRC * loadingContext;
	static HGLRC * primaryContext;
	static HDC * displayWindow;

	bool running;
};

#endif