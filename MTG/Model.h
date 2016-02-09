#ifndef MODEL_H
#define MODEL_H

#include <Windows.h>
#include <string>
#include <stdio.h>

#include "glew.h"
#include <gl/GLU.h>
#include <gl/GL.h>
#include "glm/glm.hpp"

#include "Shader.h"
#include <vector>

using std::string;
using std::vector;

class Model
{
public:
	Model();
	~Model();
	static Model * loadModel(string fileName);
	static Model * loadQuad();
	static Model * loadCardShape();
	int render();
	int getNumVertices() { return numVertices; }
	int getNumTriangles() { return numVertices/3; }
	GLuint getVertexBuffer() { return vertexBuffer; }
	GLuint getUVBuffer() { return uvBuffer; }
	GLuint getNormalBuffer() { return normalBuffer; }
private:
	static Model * loadOBJ(FILE * f);
	static Model * loadErrorModel();
	#define activeShader Shader::getActiveShader()
	int numVertices;

	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
};

#endif