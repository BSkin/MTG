#ifndef UTILITIES_H
#define UTILITIES_H

#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"

#include <string>
#include <list>
using std::string;
using std::list;

class Utilities 
{
public:
	template<typename T>
	static bool listContainsElement(list<T> * l, T element)
	{
		for (list<string>::iterator iter = l->begin(); iter != l->end(); iter++)
			if (*iter == element) return true;
		return false;
	}

	static bool pointInRect(float x, float y, float ax, float ay, float bx, float by, float dx, float dy) 
	{
		// Pseudo code
		// Corners in ax,ay,bx,by,dx,dy
		// Point in x, y

		float bax = bx - ax;
		float bay = by - ay;
		float dax = dx - ax;
		float day = dy - ay;

		if ((x - ax) * bax + (y - ay) * bay < 0.0f) return false;
		if ((x - bx) * bax + (y - by) * bay > 0.0f) return false;
		if ((x - ax) * dax + (y - ay) * day < 0.0f) return false;
		if ((x - dx) * dax + (y - dy) * day > 0.0f) return false;

		return true;
	}
	static bool pointInRect(glm::vec2 mousePoint, glm::vec2 corner1, glm::vec2 corner2, glm::vec2 corner3) 
		{ return pointInRect(mousePoint.x, mousePoint.y, corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y); }

	static glm::vec3 perspectiveToWorldPos(glm::vec3 cameraPos, glm::vec3 cameraLookAt) {
		glm::vec3 p0 = glm::vec3(0,0,0);
		glm::vec3 n = glm::vec3(0,1,0);
	
		glm::vec3 l0 = cameraPos;
		glm::vec3 l = cameraLookAt;

		// assuming vectors are all normalized
		float denom = glm::dot(n, l);
		float t = 0;
		glm::vec3 p0l0 = p0 - l0;
		t = glm::dot(p0l0, n) / denom; 
		if (t >= 0) {
			glm::vec3 p = l0 + l*t;
			glm::vec3 vec = p - p0;
			glm::vec3 up = glm::vec3(0,0,1);
			glm::vec3 side = glm::vec3(1,0,0);
			glm::vec3 upProj = up * glm::dot(vec, up) / Utilities::magnitude(up) / Utilities::magnitude(up);
			glm::vec3 sideProj = side * glm::dot(vec, side) / Utilities::magnitude(side) / Utilities::magnitude(side);

			return glm::vec3(sideProj + upProj);
		}
		return glm::vec3(0,0,0);
	}

	static glm::vec2 screenToWorldPoint(glm::vec2 pos, glm::vec2 screenSize, glm::vec2 worldSize) {
		float px = pos.x/(float)screenSize.x;
		float py = pos.y/(float)screenSize.y;
		return glm::vec2(-(px-0.5)*worldSize.x, -(py-0.5)*worldSize.y);
	}

	static glm::vec2 screenToWorldPoint(POINT pos, float screenWidth, float screenHeight, float worldX, float worldY) {
		return screenToWorldPoint(glm::vec2(pos.x, pos.y), glm::vec2(screenWidth, screenHeight), glm::vec2(worldX, worldY));
	}

	static float magSqr(glm::vec3 val) { return val.x*val.x + val.y*val.y + val.z*val.z; }
	static float magnitude(glm::vec3 val) { return sqrt(magSqr(val)); }
};


#endif