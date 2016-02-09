#ifndef SHADER_H
#define SHADER_H

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>

#include "glew.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "glm/glm.hpp"						//glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm/gtc/matrix_transform.hpp"		//glm::translate, glm::rotate, glm::scale
#include "glm/gtc/type_ptr.hpp"				//glm::value_ptr

//#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <minmax.h>

using std::string;
using std::ifstream;
using std::stringstream;
using std::list;

class Shader
{
public:
	Shader();
	~Shader();
	static Shader * getActiveShader() { return activeShader; }
	static Shader * loadShader(string name);
	int activate();
	const GLhandleARB getShaderHandle() { return shaderProgram; }

	void setUniformf1(const std::string &variable, float value);
	void setUniformf2(const std::string &variable, float value0, float value1);
	void setUniformf3(const std::string &variable, float value0, float value1, float value2);
	void setUniformf4(const std::string &variable, float value0, float value1, float value2, float value3);
	void setUniformi1(const std::string &variable, long value);
	void setUniformTexture(const std::string &variable, long textureUnit);
	void setUniformMatrixf4(const std::string &variable, const glm::mat4 &value);

private:
	static GLhandleARB makeShader(const char * fileName, GLenum type);
	static Shader * activeShader;

	string name;
	GLhandleARB vertexShader;
	GLhandleARB fragmentShader;
	GLhandleARB shaderProgram;
};

#endif