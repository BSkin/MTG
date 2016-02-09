#include "GUIItem.h"

Camera * GUIItem::camera = NULL;
InputManager * GUIItem::inputManager = NULL;
int * GUIItem::screenWidth = NULL;
int * GUIItem::screenHeight = NULL;
list<GUIItem*> * GUIItem::GUIList = NULL;

GUIItem::GUIItem(string TexturePath, string ShaderPath) : GameObject("quad.obj", TexturePath, ShaderPath)
{
	screenPos = glm::vec2(0,0);
	GUIList->push_back(this);
}

GUIItem::~GUIItem() 
{
	GUIList->remove(this);
	while (regionList.size() > 0) {
		delete regionList.front();
		regionList.pop_front();
	}
}

void GUIItem::setScreenPosition(glm::vec2 val) 
{
	if (val.x < dimensions.x/2)
		val.x = dimensions.x/2;
	if (val.x > *screenWidth - dimensions.x/2)
		val.x = *screenWidth - dimensions.x/2;
	if (val.y < dimensions.y/2)
		val.y = dimensions.y/2;
	if (val.y > *screenHeight - dimensions.y/2)
		val.y = *screenHeight - dimensions.y/2;

	screenPos = val;
}

void GUIItem::addRegion(float left, float top, float right, float bot, void (*function)())
{
	GUIRegion * newRegion = new GUIRegion(left, top, right, bot, function);
	regionList.push_back(newRegion);	
}

int GUIItem::updateGUI(long elapsedTime, int i)
{
	float x = (float)inputManager->getMousePosition().x-screenPos.x;
	float y = (float)inputManager->getMousePosition().y-screenPos.y;
	if (x < -dimensions.x/2.0f || x > dimensions.x/2.0f || y < -dimensions.y/2.0f || y > dimensions.y/2.0f)
		if (inputManager->isKeyPressed(IM_M1) || inputManager->isKeyPressed(IM_M3)) alive = false;
	if (i == -1 || i == 0) {
		if (x < -dimensions.x/2.0f || x > dimensions.x/2.0f || y < -dimensions.y/2.0f || y > dimensions.y/2.0f) {
			selected = false;
			return -1;
		}
		else {
			for (list<GUIRegion*>::iterator it = regionList.begin(); it != regionList.end(); it++) {
				if (x >= -dimensions.x/2.0f+(*it)->getLeft() && x <= -dimensions.x/2.0f+(*it)->getRight() && 
					y >= -dimensions.y/2.0f+(*it)->getTop() && y <= -dimensions.y/2.0f+(*it)->getBot()) {
					if (inputManager->isKeyPressed(IM_M1)) {
						(*it)->callFunction();
						alive = false;
					}
				}
			}
			selected = true;
			return 1;
		}
	}
	else {
		selected = false;
		return -1;
	}
}

int GUIItem::update(long elapsedTime)
{
	return GameObject::update(elapsedTime);
}

int GUIItem::render(long totalElapsedTime)
{
	glm::mat4 scaleMatrix = glm::scale(glm::vec3(dimensions.x, dimensions.y, 1.0f));
	worldMatrix = glm::inverse(glm::lookAt(glm::vec3(screenPos.x - *screenWidth/2.0f, -screenPos.y + *screenHeight/2.0f, 0), glm::vec3(screenPos.x - *screenWidth/2.0f, -screenPos.y + *screenHeight/2.0f, 1), glm::vec3(0,1,0))) * scaleMatrix; 
	normalMatrix = glm::transpose(glm::inverse(worldMatrix));
	
	if (texture == NULL || model == NULL || projMatrix == NULL || viewMatrix == NULL) return -1;
	
	Shader * temp = activeShader;
	if (!(*ignoreShader)) {
		if (shader == 0) 
			return -1;
		else shader->activate();
	}

	if (activeShader == NULL) return -1;

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
	activeShader->setUniformf1("mouseX", (float)inputManager->getMousePosition().x-screenPos.x);
	activeShader->setUniformf1("mouseY", (float)inputManager->getMousePosition().y-screenPos.y);
	activeShader->setUniformf1("width", dimensions.x);
	activeShader->setUniformf1("height", dimensions.y);
	
	model->render();

	if (!(*ignoreShader && temp != NULL)) temp->activate(); //restore old shader
}
