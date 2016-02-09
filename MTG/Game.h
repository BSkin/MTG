#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES

#include <Windows.h>

#include "glew.h"
#include <gl/GLU.h>
#include <gl/GL.h>
#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_thread.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"
#include "SDL2-2.0.3\include\SDL_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

#include "InputManager.h"
#include "Camera.h"
#include "Utilities.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "GameObject.h"
#include "Card.h"
#include "GUIItem.h"

using std::vector;

#define DEBUG 0

#if DEBUG
	#include <vld.h>	
#endif

#define FULLSCREEN	0
#define WINDOWED	1
#define BORDERLESS	2

using std::string;
using std::getline;
using std::stringstream;
using std::ostringstream;
using std::ifstream;
using std::ofstream;

class Game
{
public:
	Game(char* gameName);
	~Game(void);

	int init();
	int start();
	int cleanup();

private:
	int update(long elapsedTime);
	int renderFrame(long time);
	int render3D(long time);
	int render2D(long time);
	int loadLevel(string asdf);

	void loadConfig();
	void saveConfig();
	
	int setViewport(int width, int height);
	int resizeWindow(int width, int height);
	void startOrtho();
	void endOrtho();
	POINT getScreenSize();
	DWORD getRefreshRate();

	GLvoid buildFont();
	GLvoid killFont();
	int drawText(int x, int y, const char * s);

	int initOpenGL();
	int initGlew();
	int initSDL();
	int initObjects();
	int cleanupObjects();
	int killSDL();

	//Shortcut Functions
	inline bool isKeyDown(long e) { return inputManager.isKeyDown(e); }
	inline bool isKeyUp(long e) { return inputManager.isKeyUp(e); }
	inline bool isKeyPressed(long e) { return inputManager.isKeyPressed(e); }
	inline bool isKeyReleased(long e) { return inputManager.isKeyReleased(e); }
	inline bool isMouseScrollUp() { return inputManager.isMouseScrollUp(); }
	inline bool isMouseScrollDown() { return inputManager.isMouseScrollDown(); }

	#define activeShader Shader::getActiveShader()

	bool activeWindow, running;
	int screenWidth, screenHeight;
	short windowState;
	short bits;
	int flags;
	static int maxFps;
	char title[128];
	
	SDL_Window * displayWindow;
	SDL_GLContext displayContext;
	SDL_Thread * assetLoaderThread;
	static int startAssetLoader(void*);
	SDL_Thread * networkThread;
	static int startHostThread(void *);
	static int startClientThread(void *);

	GLuint base;
	long time;
	static InputManager inputManager;
	static AssetManager * assetManager;
	static list<GameObject *> * gameObjects;
	static list<GameObject *> * deadObjects;
	static list<GameObject*> * arenaObjects;
	static list<Card *> * cardList;
	static list<GUIItem*> * GUIList;

	double frameRate;
	#define MAXFRAMESAMPLES 100
	int tickindex;
	double ticksum;
	double ticklist[MAXFRAMESAMPLES];

	double calcFps(double newtick);

	glm::mat4 view, projection;
	Camera camera;
	bool ignoreShaders;

	int state;
	Model * quad;
	Model * card;
	Texture * startScreen;
	Shader * defaultShader;
	Shader * textShader;
	Texture * cursorTexture;
	Texture * cardBack;

	void drawCard(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Texture * t, glm::vec3 size);
	void drawRect(float x, float y, float z);
	void drawRect(float x, float y, float z, float rotation);
	void drawRect(glm::vec3 lookAt, glm::vec3 position, glm::vec3 up, glm::vec3 size);
	
	int updateSelectedCard();
	int untapAll();
	int updateCards();
	int updatePlayerCards(int PlayerID);
	int renderTooltipCard(long totalElapsedTime);
	int loadDeck(string deckName, list<string> * deck);

	bool topDownCamera;
	float topDownX, topDownY, topDownZoom;
	static int playerID;
	int numPlayers;
	bool showHand;
	bool renderArena;

	#define holdDivisor 9
	static Card * selectedCard;
	static Card * heldCard;
	static list<string> * deck;
	static list<string> * graveyard;
	static list<string> * exile;

	static void updateGraveyardTexture();
	static void updateExileTexture();

	//GUI Functions
	static void openClickMenu();
	static void openActionsMenu();
	static void openMoveMenu(); 
	static void openTokensMenu();
	static void openCountersMenu();
	static void flipSelected();
	static void tapSelected();
	static void shuffleDeck();
	static void moveToAttackers();
	static void moveToTopDeck();
	static void moveToBottomDeck();
	static void moveToHand();
	static void moveToLands();
	static void moveToPermanents();
	static void moveToGraveyard();
	static void moveToExile();
};