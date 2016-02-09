#ifndef TEXTURE_H
#define TEXTURE_H

#include <Windows.h>

#include "glew.h"
#include <gl/GLU.h>
#include <gl/GL.h>
#include "glm/glm.hpp"

#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"
#include "SDL2-2.0.3\include\SDL_image.h"

#include <string>
#include <stdio.h>

using std::string;

class Texture
{
public:
	Texture();
	~Texture();
	static Texture * loadTexture(string fileName);
	void bindTexture() { glBindTexture(GL_TEXTURE_2D, textureID); }
	GLuint getTextureID() { return textureID; } 
	glm::vec4 getPixel(float x, float y) { return glm::vec4(
		imageData[((int)(y*height)*width+(int)(x*width))*4+0], 
		imageData[((int)(y*height)*width+(int)(x*width))*4+1], 
		imageData[((int)(y*height)*width+(int)(x*width))*4+2], 
		imageData[((int)(y*height)*width+(int)(x*width))*4+3]); }
private:
	static Texture * loadSDLImage(string path);
	static Texture * loadErrorTexture();
	static Texture * loadTGA(FILE * file);
	static Texture * loadCompressedTGA(FILE * file);
	static Texture * loadUncompressedTGA(FILE * file);
	static SDL_Renderer * SDLRenderer;
	static bool generateTexture(Texture * texture, GLenum fileFormat);
	static int invertSDLImage(int pitch, int height, void * pixels);

	GLubyte * imageData;
	GLuint textureID;
	GLint type;				//GL_RGB or GL_RGBA
	GLint width, height;	//Image Dimensions
	GLint bitdepth;			//24 or 32
};


#endif 