#include "GameObject.h"

bool * GameObject::ignoreShader = NULL;
glm::mat4 * GameObject::viewMatrix = NULL;
glm::mat4 * GameObject::projMatrix = NULL;
list<GameObject *> * GameObject::gameObjectList = NULL;
AssetManager * GameObject::assetManager= NULL;

void GameObject::init()
{
	model = NULL;
	texture = NULL;
	shader = NULL;
	lighting = false;
	alive = true;
	selected = false;
	position = glm::vec3(0,0,0);
	direction = glm::vec3(0,0,-1);
	up = glm::vec3(0,1,0);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	gameObjectList->push_back(this);
}

GameObject::GameObject()
{
	init();
}

GameObject::GameObject(string mPath, string tPath, string sPath)
{
	init();

	modelPath = mPath;
	texturePath = tPath;
	shaderPath = sPath;
	model = assetManager->getModel(modelPath);
	texture = assetManager->getTexture(texturePath);
	shader = assetManager->getShader(shaderPath);

	position = glm::vec3(0,0,0);
	direction = glm::vec3(0,0,-1);
	up = glm::vec3(0,1,0);
	
	updateWorldMatrix();
}

GameObject::GameObject(string mPath, string tPath, string sPath, glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
	init();

	modelPath = mPath;
	texturePath = tPath;
	shaderPath = sPath;
	model = assetManager->getModel(modelPath);
	texture = assetManager->getTexture(texturePath);
	shader = assetManager->getShader(shaderPath);

	this->position = position;
	this->direction = direction;
	this->up = up;
	updateWorldMatrix();
}

GameObject::GameObject(string mPath, string tPath, string sPath, glm::vec3 position, glm::vec3 direction, glm::vec3 up, glm::vec3 scale)
{
	init();

	modelPath = mPath;
	texturePath = tPath;
	shaderPath = sPath;
	model = assetManager->getModel(modelPath);
	texture = assetManager->getTexture(texturePath);
	shader = assetManager->getShader(shaderPath);

	this->position = position;
	this->direction = direction;
	this->up = up;
	this->scale = scale;
	updateWorldMatrix();
}


GameObject::~GameObject()
{
	
}

void GameObject::updateWorldMatrix() 
{
	worldMatrix = glm::inverse(glm::lookAt(position, position+direction, up)) * glm::scale(glm::mat4(), scale);
	normalMatrix = glm::transpose(glm::inverse(worldMatrix));
}

int GameObject::update(long elapsedTime)
{
	if (model == NULL) {
		model = assetManager->getModel(modelPath);
		if (model == NULL) {
			assetManager->forceLoadModel(modelPath);
			model = assetManager->getModel(modelPath);
		}
	}
	if (texture == NULL) {
		texture = assetManager->getTexture(texturePath);
		if (texture == NULL) {
			assetManager->forceLoadTexture(texturePath);
			texture = assetManager->getTexture(texturePath);
		}
	}
	if (shader == NULL) {
		shader = assetManager->getShader(shaderPath);
		if (shader == NULL) {
			assetManager->forceLoadShader(shaderPath);
			shader = assetManager->getShader(shaderPath);
		}
	}
	return 0;
}

int GameObject::render(long totalElapsedTime)
{
	if (texture == NULL || model == NULL || projMatrix == NULL || viewMatrix == NULL) 
		return -1;;
	
	Shader * temp = activeShader;
	if (!(*ignoreShader)) {
		if (shader == 0) 
			return -1;
		else shader->activate();
	}

	if (activeShader == NULL) 
		return -1;

	activeShader->setUniformTexture("texS", texture->getTextureID());
	glActiveTexture(GL_TEXTURE0);
	texture->bindTexture();
	glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "texS"), 0);

	activeShader->setUniformMatrixf4("worldViewProj", *projMatrix * *viewMatrix * worldMatrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);
	activeShader->setUniformf1("time", totalElapsedTime);
	if (lighting)	activeShader->setUniformf1("enableLighting", 1.0f);
	else			activeShader->setUniformf1("enableLighting", 0.0f);
	if (selected)	activeShader->setUniformf1("selectedCard", 1.0f);
	else			activeShader->setUniformf1("selectedCard", 0.0f);
	
	model->render();

	if (!(*ignoreShader && temp != NULL)) temp->activate(); //restore old shader
}