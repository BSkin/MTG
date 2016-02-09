#include "AssetLoader.h"

HGLRC * AssetLoader::loadingContext = NULL;
HGLRC * AssetLoader::primaryContext = NULL;
HDC * AssetLoader::displayWindow = NULL;

AssetLoader::AssetLoader()
{

}

AssetLoader::~AssetLoader()
{

}

void AssetLoader::start()
{
	while (running)
	{
		DWORD waitResult = WaitForSingleObject(objectsMutex, INFINITE);
		if (objectsToLoad->size() == 0) {
			if (! ReleaseMutex(objectsMutex)) printf("Error releasing Objects Mutex"); 
			Sleep(100);
		}
		else {
			string path = objectsToLoad->front(); objectsToLoad->pop_front();
			if (! ReleaseMutex(objectsMutex)) printf("Error releasing Objects Mutex"); 
			
			//Find out Type
			string tmp = path;
			string rev = "";
			while (tmp.back() != '.') {
				rev += tmp.back();
				tmp.pop_back();
			}
			tmp = "";
			while (rev.size() > 0) {
				tmp += rev.back();
				rev.pop_back();
			}

			if (tmp == "obj")		loadModel(path);
			else if (tmp == "tga")	loadTexture(path);
			else if (tmp == "jpg")	loadTexture(path);
			else if (tmp == "png")	loadTexture(path);
			else if (tmp == "glsl")	loadShader(path);

		}
	}
}

void AssetLoader::loadModel(string path)
{
	DWORD waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		modelMap::iterator it = models->find(path);
		bool newItem = (it == models->end());
	if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 
	
	if (!newItem) return;
	Model * temp = Model::loadModel(path);

	waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		models->insert(std::pair<string, Model*>(path, temp));
	if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 
}

void AssetLoader::loadTexture(string path)
{
	DWORD waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		textureMap::iterator it = textures->find(path);
		bool newItem = (it == textures->end());
	if (! ReleaseMutex(texturesMutex)) printf("Error releasing texture Mutex"); 
	
	if (!newItem) return;
	Texture * temp = Texture::loadTexture(path);

	waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		textures->insert(std::pair<string, Texture*>(path, temp));
	if (! ReleaseMutex(texturesMutex)) printf("Error releasing texture Mutex"); 
}

void AssetLoader::loadShader(string path)
{
	DWORD waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		shaderMap::iterator it = shaders->find(path);
		bool newItem = (it == shaders->end());
	if (! ReleaseMutex(shadersMutex)) printf("Error releasing shaders Mutex"); 
	
	if (!newItem) return;
	Shader * temp = Shader::loadShader(path);

	waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		shaders->insert(std::pair<string, Shader*>(path, temp));
	if (! ReleaseMutex(shadersMutex)) printf("Error releasing shaders Mutex"); 
}