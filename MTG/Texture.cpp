#include "Texture.h"

Texture::Texture()
{
	imageData		= 0;
	textureID		= 0;
	type			= -1;
	width = height	= -1;
	bitdepth		= -1;
}

Texture::~Texture()
{
	
	if (imageData != 0) delete [] imageData;
	if (textureID != 0) glDeleteTextures (1, &textureID);
}

Texture * Texture::loadTexture(string path)
{
 	FILE * file = 0;
	file = fopen(path.c_str(), "rb");
	if (file == NULL) 
		return loadErrorTexture();	//File not found, generate error texture
	
	if (strcmp(path.c_str() + path.size()-4, ".tga") == 0 || strcmp(path.c_str() + path.size()-4, ".TGA") == 0)
		return loadTGA(file);

	return loadSDLImage(path);

//	return loadErrorTexture(); //not an accepted filetype
	
}

int Texture::invertSDLImage(int pitch, int height, void * pixels)
{
	int index;
	void* temp_row;
	int height_div_2;

	temp_row = (void *)malloc(pitch);
	if(NULL == temp_row)
	{
		SDL_SetError("Not enough memory for image inversion");
		return -1;
	}
	//if height is odd, don't need to swap middle row
	height_div_2 = (int) (height * .5);
	for(index = 0; index < height_div_2; index++) 	{
		//uses string.h
		memcpy((Uint8 *)temp_row,
			(Uint8 *)(pixels) +
			pitch * index,
			pitch);

		memcpy(
			(Uint8 *)(pixels) +
			pitch * index,
			(Uint8 *)(pixels) +
			pitch * (height - index-1),
			pitch);
		memcpy(
			(Uint8 *)(pixels) +
			pitch * (height - index-1),
			temp_row,
			pitch);
	}
	free(temp_row);
	return 0;
}

Texture * Texture::loadSDLImage(string path) {
	SDL_Surface * texture = IMG_Load(path.c_str());
	if(texture == NULL) {
		return loadErrorTexture();
	}

	Texture * temp = new Texture();
	if (temp == NULL) return loadErrorTexture();

	string id = "";
	if (strcmp(path.c_str() + path.size()-4, ".png") == 0 || strcmp(path.c_str() + path.size()-4, ".PNG") == 0)
		id = "png";
	else if (strcmp(path.c_str() + path.size()-4, ".jpg") == 0 || strcmp(path.c_str() + path.size()-4, ".JPG") == 0)
		id = "jpg";

	temp->width = texture->w;
	temp->height = texture->h;
	temp->bitdepth = texture->format->BitsPerPixel;

	invertSDLImage(texture->pitch, texture->h, texture->pixels);

	temp->imageData = new GLubyte[temp->width * temp->height * temp->bitdepth/8];
	int bytesPerPixel = temp->bitdepth/8;

	/*for (int x = 0; x < temp->width; x++) {
		for (int y = 0; y < temp->height; y++) {
			temp->imageData[(y*temp->width + x)*bytesPerPixel +0] = ((GLubyte*)texture->pixels)[((temp->height-y)*temp->width + x)*bytesPerPixel +0];
			temp->imageData[(y*temp->width + x)*bytesPerPixel +1] = ((GLubyte*)texture->pixels)[((temp->height-y)*temp->width + x)*bytesPerPixel +1];
			temp->imageData[(y*temp->width + x)*bytesPerPixel +2] = ((GLubyte*)texture->pixels)[((temp->height-y)*temp->width + x)*bytesPerPixel +2];

			if (bytesPerPixel > 3)
				temp->imageData[(y*temp->width + x)*bytesPerPixel +3] = ((GLubyte*)texture->pixels)[((temp->height-y)*temp->width + x)*bytesPerPixel +3];
		}
	}

	for (int x = 0; x < temp->width; x++) {
		for (int y = 0; y < temp->height; y++) {
			temp->imageData[(y*temp->width + x)*bytesPerPixel +0] = ((GLubyte*)texture->pixels)[(y*temp->width + x)*bytesPerPixel +0];
			temp->imageData[(y*temp->width + x)*bytesPerPixel +1] = ((GLubyte*)texture->pixels)[(y*temp->width + x)*bytesPerPixel +1];
			temp->imageData[(y*temp->width + x)*bytesPerPixel +2] = ((GLubyte*)texture->pixels)[(y*temp->width + x)*bytesPerPixel +2];

			if (bytesPerPixel > 3)
				temp->imageData[(y*temp->width + x)*bytesPerPixel +3] = ((GLubyte*)texture->pixels)[(y*temp->width + x)*bytesPerPixel +3];
		}
	}*/

	for (int i = 0; i < temp->width * temp->height * bytesPerPixel; i++) temp->imageData[i] = ((GLubyte*)texture->pixels)[i];
	
 	if (bytesPerPixel == 4) {
		temp->type = GL_RGBA;
		generateTexture(temp, GL_RGBA);
	}
	else if (bytesPerPixel == 3) {
		temp->type = GL_RGB;
		generateTexture(temp, GL_RGB);
	}

	//clean the SDL surface
	SDL_FreeSurface(texture);

	return temp;
}

Texture * Texture::loadErrorTexture()
{
	Texture * t = new Texture();

	t->type = GL_RGBA;
	t->width = t->height = 512;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageData = new GLubyte[imageSize];

	for (int x = 0; x < t->width; x++)
	{
		for (int y = 0; y < t->height; y++)
		{
			if ((int)(x/64+y/64) % 2 == 1)
			{
				t->imageData[(y*t->width+x)*4+0] = 255;
				t->imageData[(y*t->width+x)*4+1] = 255;
				t->imageData[(y*t->width+x)*4+2] = 255;
				t->imageData[(y*t->width+x)*4+3] = 255;
			}
			else 
			{
				t->imageData[(y*t->width+x)*4+0] = 226;
				t->imageData[(y*t->width+x)*4+1] = 156;
				t->imageData[(y*t->width+x)*4+2] = 210;
				t->imageData[(y*t->width+x)*4+3] = 255;
			}
		}
	}

	generateTexture(t, t->type);

	return t;
}

Texture * Texture::loadTGA(FILE * file)
{
	GLbyte header[12];

	// Uncompressed TGA Header
	GLubyte uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
	// Compressed TGA Header
	GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

	// Attempt To Read The File Header
	if (fread(&header, sizeof(GLbyte)*12, 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header */ } 

	// If The File Header Matches The Uncompressed Header
	if(memcmp(uTGAcompare, &header, sizeof(header)) == 0)
		return loadUncompressedTGA(file);

	// If The File Header Matches The Compressed Header
	else if(memcmp(cTGAcompare, &header, sizeof(header)) == 0)
		return loadCompressedTGA(file);

	return loadErrorTexture(); //error checking the type of TGA
}

Texture * Texture::loadUncompressedTGA(FILE * file)
{
	GLbyte header[12];

	// Attempt To Read Next 6 Bytes
	fseek(file, 12, 0);
	if (fread(header, sizeof(header), 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header*/ }

	Texture * t = new Texture();
	if (t == NULL) { return loadErrorTexture(); /* Not properly allocated */ }

	t->width  = header[1] * 256 + header[0];
	t->height = header[3] * 256 + header[2];
	t->bitdepth = header[4];

	// Make Sure All Information Is Valid
	if ((t->width <= 0) || (t->height <= 0) || ((t->bitdepth != 24) && (t->bitdepth !=32))) { delete t; return loadErrorTexture(); /* Bad Data*/ }

	if (t->bitdepth == 24)	t->type = GL_RGB;
	else					t->type = GL_RGBA;

	GLint bytesPerPixel = (t->bitdepth / 8);				// Calculate The BYTES Per Pixel
	GLint imageSize = bytesPerPixel * t->width * t->height;	// Calculate Memory Needed To Store Image

	// Allocate Memory
	t->imageData = new GLubyte[imageSize];
	if (t->imageData == NULL) { delete t; return loadErrorTexture(); /* Not Properly Allocated */ }

	// Attempt To Read All The Image Data
	fseek(file, 18, 0);
	if (fread(t->imageData, 1, imageSize, file) != imageSize) { delete t; return false; /* Can't read the file*/	}

	if (t->type == GL_RGBA)
	{
		//swap 1st and 3rd bytes (R and B)
	    for(GLuint cswap = 0; cswap < (int)imageSize; cswap += bytesPerPixel)
	    {
	        // 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
	        t->imageData[cswap] ^= t->imageData[cswap+2] ^=
	        t->imageData[cswap] ^= t->imageData[cswap+2];
	    }
	}
 
    fclose(file);
	if (t->type == GL_RGB) generateTexture(t, GL_BGR);
	else if (t->type == GL_RGBA) generateTexture(t, GL_RGBA);
	return t;
}

Texture * Texture::loadCompressedTGA(FILE * file)
{
	GLbyte header[12];

	// Attempt To Read Next 6 Bytes
	fseek(file, 12, 0);
	if (fread(header, sizeof(header), 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header*/ }

	Texture * t = new Texture();
	if (t == NULL) { return loadErrorTexture(); /* Not properly allocated */ }

	t->width  = header[1] * 256 + header[0];
    t->height = header[3] * 256 + header[2];
    t->bitdepth = header[4];
	if ((t->width <= 0) || (t->height <= 0) || ((t->bitdepth != 24) && (t->bitdepth !=32))) { delete t; return loadErrorTexture(); }  

	if (t->bitdepth == 24)	t->type = GL_RGB;
	else					t->type = GL_RGBA;

	GLint bytesPerPixel = (t->bitdepth / 8);				// Calculate The BYTES Per Pixel
	GLint imageSize = bytesPerPixel * t->width * t->height;	// Calculate Memory Needed To Store Image

	t->imageData = new GLubyte[imageSize];
	if (t->imageData == NULL) { delete t; return loadErrorTexture(); /* Not Properly Allocated */ }

	GLuint pixelcount = t->height * t->width; // Number Of Pixels In The Image
	GLuint currentpixel = 0;            // Current Pixel We Are Reading From Data
	GLuint currentbyte  = 0;            // Current Byte We Are Writing Into Imagedata
	// Storage For 1 Pixel
	GLubyte * colorbuffer = new GLubyte[bytesPerPixel];

	fseek(file, 18, 0);

	do                      // Start Loop
	{
		GLubyte chunkheader = 0;            // Variable To Store The Value Of The Id Chunk
		if (fread(&chunkheader, sizeof(GLubyte), 1, file) == 0) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed to load chunk header */ }
	
		if(chunkheader < 128)                // If The Chunk Is A 'RAW' Chunk
		{                                                  
			chunkheader++;              // Add 1 To The Value To Get Total Number Of Raw Pixels

			// Start Pixel Reading Loop
			for(short counter = 0; counter < chunkheader; counter++)
			{
				// Try To Read 1 Pixel
				if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed To read File*/ }

				t->imageData[currentbyte] = colorbuffer[2];        // Write The 'R' Byte
				t->imageData[currentbyte + 1   ] = colorbuffer[1]; // Write The 'G' Byte
				t->imageData[currentbyte + 2   ] = colorbuffer[0]; // Write The 'B' Byte
				if(bytesPerPixel == 4)                  // If It's A 32bpp Image...
				{
					t->imageData[currentbyte + 3] = colorbuffer[3];    // Write The 'A' Byte
				}
				// Increment The Byte Counter By The Number Of Bytes In A Pixel
				currentbyte += bytesPerPixel;
				currentpixel++;                 // Increment The Number Of Pixels By 1
			}
		}
		else                        // If It's An RLE Header
		{
			chunkheader -= 127;         // Subtract 127 To Get Rid Of The ID Bit

			// Read The Next Pixel
			if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed To read File*/ }

			// Start The Loop
			for(short counter = 0; counter < chunkheader; counter++)
			{
				// Copy The 'R' Byte
				t->imageData[currentbyte] = colorbuffer[2];
				// Copy The 'G' Byte
				t->imageData[currentbyte + 1   ] = colorbuffer[1];
				// Copy The 'B' Byte
				t->imageData[currentbyte + 2   ] = colorbuffer[0];
				if(bytesPerPixel == 4)      // If It's A 32bpp Image
				{
					// Copy The 'A' Byte
					t->imageData[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += bytesPerPixel;   // Increment The Byte Counter
				currentpixel++;             // Increment The Pixel Counter
			}
		}
	}    while(currentpixel < pixelcount);    // More Pixels To Read? ... Start Loop Over
    fclose(file);               // Close File

	if (t->type == GL_RGB) generateTexture(t, GL_BGR);
	else if (t->type == GL_RGBA) generateTexture(t, GL_RGBA);

	return t;
}

bool Texture::generateTexture(Texture * t, GLenum fileFormat)
{
    glGenTextures(1, &t->textureID);
    glBindTexture(GL_TEXTURE_2D, t->textureID);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	const unsigned char * ver = glGetString(GL_VERSION);
	string v = "";
	int i = 0;
	while (ver[i] != ' ') { v += ver[i]; i++; }

	float version = atof(v.c_str());
	
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D, 0, t->type, 
		t->width, t->height,
		0,
		fileFormat, GL_UNSIGNED_BYTE, t->imageData
	);

	if (version >= 3.0)
	{	
		glGenerateMipmap(GL_TEXTURE_2D); 

		/*glGenTextures(1, &t->textureID);
		glBindTexture(GL_TEXTURE_2D, t->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, t->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/
	}
	else if (version >= 1.4)
	{
		glGenTextures(1, &t->textureID);
		glBindTexture(GL_TEXTURE_2D, t->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, t->imageData);
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, t->width, t->height, GL_BGRA, GL_UNSIGNED_BYTE, t->imageData);
	}

    glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}