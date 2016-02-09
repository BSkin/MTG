#include "AssetManager.h"

AssetManager::AssetManager() : assetLoader(), objectsToLoad(), models(), textures(), shaders(), objectsMutex(NULL), modelsMutex(NULL), texturesMutex(NULL), shadersMutex(NULL)
{
	objectsMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

	modelsMutex = CreateMutex(NULL, FALSE, NULL);
	if (modelsMutex == NULL) printf("CreateMutex error: %d\n", GetLastError());
	texturesMutex = CreateMutex(NULL, FALSE, NULL);
	if (texturesMutex == NULL) printf("CreateMutex error: %d\n", GetLastError());
	shadersMutex = CreateMutex(NULL, FALSE, NULL);
	if (shadersMutex == NULL) printf("CreateMutex error: %d\n", GetLastError());

	assetLoader.objectsToLoad = &objectsToLoad;
	assetLoader.models = &models;
	assetLoader.textures = &textures;
	assetLoader.shaders = &shaders;
	assetLoader.objectsMutex = &objectsMutex;
	assetLoader.modelsMutex = &modelsMutex;
	assetLoader.texturesMutex = &texturesMutex;
	assetLoader.shadersMutex = &shadersMutex;
}

AssetManager::~AssetManager()
{
	objectsToLoad.clear();

	//DWORD waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		for (modelMap::iterator it = models.begin(); it != models.end(); it++) {
			delete it->second;
			it->second = 0;
		}
		models.clear();
	//if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 

	//waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		for (textureMap::iterator it = textures.begin(); it != textures.end(); it++) {
			delete it->second;
			it->second = 0;
		}
		textures.clear();
	//if (! ReleaseMutex(texturesMutex)) printf("Error releasing Textures Mutex"); 

	//waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		for (shaderMap::iterator it = shaders.begin(); it != shaders.end(); it++) {
			delete it->second;
			it->second = 0;
		}
		shaders.clear();
	//if (! ReleaseMutex(shadersMutex)) printf("Error releasing Textures Mutex"); 

	if (objectsMutex != NULL)	CloseHandle(objectsMutex);
	if (modelsMutex != NULL)	CloseHandle(modelsMutex);
	if (texturesMutex != NULL)	CloseHandle(texturesMutex);
	if (shadersMutex != NULL)	CloseHandle(shadersMutex);
}

void AssetManager::startAssetLoader()
{
	assetLoader.start();
}

void AssetManager::pushToObjectsList(string path)
{
	DWORD waitResult = WaitForSingleObject(objectsMutex, INFINITE);
		if (!Utilities::listContainsElement(&objectsToLoad, path))
			objectsToLoad.push_back(path);
	if (! ReleaseMutex(objectsMutex)) printf("Error releasing Objects Mutex"); 
}

void AssetManager::loadModel(string path)
{
	DWORD waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		modelMap::iterator it = models.find(path);
	if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 
	
	if (it == models.end())
		pushToObjectsList(path);
}

void AssetManager::loadTexture(string path)
{
	DWORD waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		textureMap::iterator it = textures.find(path);
	if (! ReleaseMutex(texturesMutex)) printf("Error releasing Textures Mutex"); 
	
	if (it == textures.end())
		pushToObjectsList(path);
}

void AssetManager::loadShader(string path)
{
	DWORD waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		shaderMap::iterator it = shaders.find(path);
	if (! ReleaseMutex(shadersMutex)) printf("Error releasing Shaders Mutex"); 
	
	if (it == shaders.end())
		pushToObjectsList(path);
}

void AssetManager::forceLoadModel(string path)
{
	DWORD waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		modelMap::iterator it = models.find(path);	
		if (it == models.end())
			models.insert(std::pair<string, Model*>(path, Model::loadModel(path)));
	if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 
}

void AssetManager::forceLoadTexture(string path)
{
	DWORD waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		textureMap::iterator it = textures.find(path);	
		if (it == textures.end())
			textures.insert(std::pair<string, Texture*>(path, Texture::loadTexture(path)));
	if (! ReleaseMutex(texturesMutex)) printf("Error releasing Textures Mutex"); 
}

void AssetManager::forceLoadShader(string path)
{
	DWORD waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		shaderMap::iterator it = shaders.find(path);	
		if (it == shaders.end())
			shaders.insert(std::pair<string, Shader*>(path, Shader::loadShader(path)));
	if (! ReleaseMutex(shadersMutex)) printf("Error releasing Shaders Mutex"); 
}

Model * AssetManager::getModel(string path)
{
	DWORD waitResult = WaitForSingleObject(modelsMutex, INFINITE);
		modelMap::iterator it = models.find(path);
	if (! ReleaseMutex(modelsMutex)) printf("Error releasing Models Mutex"); 
	
	if (it == models.end()) {
		pushToObjectsList(path);
		return NULL;
	}
	return it->second;
}

Texture * AssetManager::getTexture(string path)
{
	DWORD waitResult = WaitForSingleObject(texturesMutex, INFINITE);
		textureMap::iterator it = textures.find(path);
	if (! ReleaseMutex(texturesMutex)) printf("Error releasing Textures Mutex"); 
	
	if (it == textures.end()) {
		pushToObjectsList(path);
		return NULL;
	}
	return it->second;
}

Shader * AssetManager::getShader(string path)
{
	DWORD waitResult = WaitForSingleObject(shadersMutex, INFINITE);
		shaderMap::iterator it = shaders.find(path);
	if (! ReleaseMutex(shadersMutex)) printf("Error releasing Shaders Mutex"); 
	
	if (it == shaders.end()) {
		pushToObjectsList(path);
		return NULL;
	}
	return it->second;
}