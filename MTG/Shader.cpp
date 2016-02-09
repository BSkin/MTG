#include "Shader.h"

Shader * Shader::activeShader = NULL;

Shader::Shader()
{
	name = "";
	shaderProgram = 0;
	vertexShader = 0;
	fragmentShader = 0;
}

Shader::~Shader()
{
	glDetachShader(shaderProgram, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
	
	//if (shaderProgram != 0) glDeleteObjectARB (shaderProgram); //Should also detach the attached handles..
	//if (vertexShader != 0) glDeleteObjectARB (vertexShader);
	//if (fragmentShader != 0) glDeleteObjectARB (fragmentShader);
}

int Shader::activate()
{
	if (activeShader == this) 
		return 1;

	activeShader = this;
	glUseProgramObjectARB(shaderProgram);	

	//glUniform3f(glGetUniformLocation(shaderProgram, "worldViewProj"), 

	return 0;
}

Shader * Shader::loadShader(string name)
{
	Shader * s = new Shader();
	
	for (int i = 0; i < 5; i++) name.pop_back();
	s->name = name;

	string vertPath = name + ".vert";
	string fragPath = name + ".frag";

	//Start with a handle for the shader program...
	GLhandleARB shaderProgramHandle = glCreateProgramObjectARB ();

	//glBindAttribLocation (shaderProgramHandle, 0, "mg_vertex");
	//glBindAttribLocation (shaderProgramHandle, 1, "uv");
	// use glGetAttribLocation  instead

	GLhandleARB vertObject = makeShader(vertPath.c_str(), GL_VERTEX_SHADER);
	GLhandleARB fragObject = makeShader(fragPath.c_str(), GL_FRAGMENT_SHADER);
	glAttachObjectARB(shaderProgramHandle, vertObject);
	glAttachObjectARB(shaderProgramHandle, fragObject);

	glLinkProgramARB (shaderProgramHandle);
	
	//Find out if compilation worked and return the program handle if it did...
	int status; glGetObjectParameterivARB (shaderProgramHandle, GL_OBJECT_LINK_STATUS_ARB, &status);
	if (status != 0) 
	{
		//return shaderProgramHandle; //Everything OK...
		s->shaderProgram = shaderProgramHandle;
		s->vertexShader = vertObject;
		s->fragmentShader = fragObject;

		return s;
	}

	//It didn't, so log error information...
	//::log ("\nFailed to link shader \"%s\"...", name.c_str ());
	int length = 0; 
	glGetObjectParameterivARB (shaderProgramHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
	glBindFragDataLocation(shaderProgramHandle, 0, "outColour");

	const long MAXIMUM_LOG_STRING = 1024; char logString [MAXIMUM_LOG_STRING];
	GLsizei messageLength = min (length, MAXIMUM_LOG_STRING);
	if (messageLength > 0) {
		glGetInfoLogARB (shaderProgramHandle, messageLength, 0, logString);
	}

	//and detach what was previously attached and discard the program handle that was obtained...
	glDetachObjectARB(shaderProgramHandle, fragObject);
	glDetachObjectARB(shaderProgramHandle, vertObject);

	glDeleteObjectARB (shaderProgramHandle); //Should also detach the attached handles...

	return NULL;
}

GLhandleARB Shader::makeShader(const char * fileName, GLenum type)
{
	ifstream file(fileName);
	stringstream ss;
	ss << file.rdbuf();
	string source = ss.str();
	char * mem = (char *)alloca(source.length()+1);
	strcpy(mem, source.c_str());
	//mem[source.length] = '\0';

	GLhandleARB shaderHandle = glCreateShaderObjectARB (type);
	glShaderSourceARB (shaderHandle, 1, (const GLcharARB **)&mem, 0);
	glCompileShaderARB (shaderHandle);
	int status; glGetObjectParameterivARB (shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);
	if (status != 0) 
		return shaderHandle; //Everything OK...

	//It didn't, so log error information...
	//::log ("\nFailed to compile %s shader \"%s\"...", type == GL_VERTEX_SHADER_ARB ? "VERTEX" : "PIXEL", name.c_str ());
	int length = 0; glGetObjectParameterivARB (shaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
	const long MAXIMUM_LOG_STRING = 1024; char logString [MAXIMUM_LOG_STRING];
	GLsizei messageLength = min (length, MAXIMUM_LOG_STRING);
	if (messageLength > 0) {
		glGetInfoLogARB (shaderHandle, messageLength, 0, logString);
		MessageBox(NULL, logString, "Shader Error", MB_OK);
	}

	glDeleteObjectARB (shaderHandle); 

	return NULL;
}

void Shader::setUniformf1(const std::string &variable, float value) {
	if (shaderProgram != -1) 
		glUniform1fARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), value);
}

void Shader::setUniformf2(const std::string &variable, float value0, float value1) {
	if (shaderProgram != -1) 
		glUniform2fARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), value0, value1);
}

void Shader::setUniformf3(const std::string &variable, float value0, float value1, float value2) {
	if (shaderProgram != -1) 
		glUniform3fARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), value0, value1, value2);
}

void Shader::setUniformf4(const std::string &variable, float value0, float value1, float value2, float value3) {
	if (shaderProgram != -1) 
		glUniform4fARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), value0, value1, value2, value3);
}

void Shader::setUniformi1(const std::string &variable, long value) {
	if (shaderProgram != -1) 
		glUniform1iARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), value);
}

void Shader::setUniformTexture(const std::string &variable, long textureUnit)
{
	if (shaderProgram != -1) 
		glUniform1iARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), textureUnit);
}

void Shader::setUniformMatrixf4(const std::string &variable, const glm::mat4 &value)
{
	const long matrixCount = 1; const bool shouldTranspose = GL_FALSE;
	if (shaderProgram != -1) 
		glUniformMatrix4fvARB (glGetUniformLocationARB (shaderProgram, variable.c_str ()), matrixCount, shouldTranspose, glm::value_ptr(value));
}