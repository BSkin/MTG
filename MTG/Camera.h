#ifndef CAMERA_H
#define CAMERA_H

#include <Windows.h>
#include <math.h>
#include <gl/GLU.h>
#include <gl/GL.h>

#include "glm/glm.hpp"						//glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm/gtc/matrix_transform.hpp"		//glm::translate, glm::rotate, glm::scale
#include "glm/gtc/type_ptr.hpp"				//glm::make_mat4
#include "glm/gtx/rotate_vector.hpp"

#include "Utilities.h"

class Camera
{
public:
	Camera();
	~Camera();

	int calculateView(glm::mat4 * view);
	int calculateMenuView(glm::mat4 * view);
	int calculateProjection(glm::mat4 * proj);
	int calculateOrthographicProjection(glm::mat4 * view, float width, float height);

	void move(glm::vec3 value) { position += value; }
	void move(float x, float y, float z) { move(glm::vec3(x, y, z)); } 
	void move(float x, float z) { move(x, 0, z); }
	void moveForward(float speed) { position += lookAtVector * speed; }
	void moveBack(float speed) { position -= lookAtVector * speed; }
	void moveLeft(float speed) { position += leftVector * speed; }
	void moveRight(float speed) { position -= leftVector * speed; }
	void moveUp(float speed) { position += upVector * speed; }
	void moveDown(float speed) { position -= upVector * speed; }
	void setPosition(glm::vec3 value) { position = value; }
	void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }
	void zeroVelocity() { velocity = glm::vec3(0,0,0); }
	void turn(long x, long y);
	void turn(POINT v) { turn(v.x, v.y); }
	void roll(float z);
	void toggleFreeRotation();
	void setFreeRotation(bool x) { if (freeRotation != x) toggleFreeRotation(); }
	void setUpVector(glm::vec3 x) { upVector = x; }
	void setUpVector(float x, float y, float z) { setUpVector(glm::vec3(x, y, z)); }
	void setLookAtPoint(glm::vec3 x) { if (x != position) lookAtVector = glm::normalize(x - position); }
	void setLookAtPoint(float x, float y, float z) { setLookAtPoint(glm::vec3(x, y, z)); }
	void setLookAtVector(glm::vec3 x) { if (x != glm::vec3(0,0,0)) lookAtVector = glm::normalize(x); }
	void setLookAtVector(float x, float y, float z) { setLookAtVector(glm::vec3(x, y, z)); }

	float getAspectRatio() { return aspectRatio; }
	void setAspectRatio(float x) { aspectRatio = x; }
	glm::vec3 getPos()			{ return position; }
	glm::vec3 getAngle()		{ return glm::vec3(xAngle, yAngle, zAngle); }
	glm::vec3 getLookAtVector()	{ return lookAtVector; }
	glm::vec3 getUpVector()		{ return upVector; }
	glm::vec3 getLeftVector()	{ return leftVector; }

	void update(long dt);
private:
	inline void calculateLeftVector() { leftVector = glm::normalize(glm::cross(upVector, lookAtVector)); }

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 lookAtVector;
	glm::vec3 upVector;
	glm::vec3 leftVector;

	float vFOV, aspectRatio, nearPlane, farPlane;
	bool freeRotation;
	float xAngle, yAngle, zAngle;
};

#endif