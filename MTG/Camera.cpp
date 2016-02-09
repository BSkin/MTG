#include "Camera.h"

Camera::Camera() 
{
	nearPlane = 0.1f;
	farPlane = 100.0f;
	vFOV = 40.0f;
	aspectRatio = 16.0f/9.0f;
	freeRotation = false;
	
	position = glm::vec3(0,2,-4);
	velocity = glm::vec3(0,0,0);

	lookAtVector = glm::normalize(glm::vec3(0,-1,2));
	upVector = glm::vec3(0,1,0);
	leftVector = glm::vec3(1,0,0);
	xAngle = yAngle = zAngle = 0.0f;
}

Camera::~Camera()
{

}

void Camera::update(long dt)
{
	//position += velocity * (float)dt/1000.0f;
}

void Camera::toggleFreeRotation()
{
	freeRotation = !freeRotation;
	if (!freeRotation) {
		float angle = acos((lookAtVector.x*upVector.x + lookAtVector.y*upVector.y + lookAtVector.z*upVector.z) / (Utilities::magnitude(lookAtVector) * Utilities::magnitude(upVector)));
		xAngle = 0.0f;
		yAngle = angle*180.0f/M_PI - 90.0f;
		calculateLeftVector();
	}
}

void Camera::turn(long x, long y)
{
	float yVal = y * 0.1f;
	float xVal = x * 0.1f;

	if (!freeRotation)
	{
		if		(yAngle + yVal < -89.9f)	yVal = -89.9f - yAngle;
		else if (yAngle + yVal > 89.9f)		yVal = 89.9f - yAngle;
		yAngle += yVal;

		xAngle += xVal;
		if (xAngle > 180) xAngle -= 360.0f;
		if (xAngle < -180) xAngle += 360.0f;
	}
	
	lookAtVector = glm::normalize(glm::rotate(lookAtVector, yVal, leftVector));
	if (freeRotation) upVector = glm::normalize(glm::cross(lookAtVector, leftVector));

	lookAtVector = glm::normalize(glm::rotate(lookAtVector, xVal, -upVector));
	calculateLeftVector();
}

void Camera::roll(float z)
{
	if (!freeRotation) return;

	zAngle += z;
	if (zAngle > 180) zAngle -= 360;
	if (zAngle < -180) zAngle += 360;

	upVector = glm::rotate(upVector, z, lookAtVector);
	leftVector = glm::normalize(glm::cross(upVector, lookAtVector));
}

int Camera::calculateView(glm::mat4 * view)
{
	*view = glm::lookAt(position, position+lookAtVector, upVector); 
	return 0;
}

int Camera::calculateMenuView(glm::mat4 * view)
{
	*view = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0)); 
	return 0;
}

int Camera::calculateProjection(glm::mat4 * proj)
{
	*proj = glm::perspective(vFOV, aspectRatio, nearPlane, farPlane);
	return 0;
}

int Camera::calculateOrthographicProjection(glm::mat4 * proj, float width, float height)
{
	*proj = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -10.0f, 100.0f);
	return 0;
}