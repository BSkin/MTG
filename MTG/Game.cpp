#include "Game.h"

#pragma region Static Initialization
AssetManager * Game::assetManager = NULL;
InputManager Game::inputManager;
list<GameObject *> * Game::gameObjects = NULL;
list<GameObject *> * Game::deadObjects = NULL;
list<Card *> * Game::cardList = NULL;
list<GUIItem*> * Game::GUIList = NULL;
list<GameObject*> * Game::arenaObjects = NULL;
Card * Game::selectedCard = NULL;
Card * Game::heldCard = NULL;
list<string> * Game::deck = NULL;
list<string> * Game::graveyard = NULL;
list<string> * Game::exile = NULL;
int Game::playerID = 0;
int Game::maxFps = 0;
#pragma endregion

Game::Game(char* gameName)
{
	title[127] = '\0';
	if (title == NULL) this->title[0] = '\0';
	else strncpy(this->title, gameName,127);
	frameRate = 0;
	activeWindow = false;
	running = true;
	screenWidth	= 1;
	screenHeight = 1;
	windowState	= WINDOWED;
	bits = 32;
	flags = 0;
	maxFps = 300;
	title[128];
	base = 0;
	ignoreShaders = false;
	numPlayers = 2;
	
	displayWindow =	NULL;
	displayContext = NULL;
	assetLoaderThread = NULL;

	state = 0;
	selectedCard = NULL;
	showHand = true;

	srand(GetCurrentTime());

	tickindex=ticksum=0;
	for (int i = 0; i < MAXFRAMESAMPLES; i++) ticklist[i] = 0;
}

Game::~Game()
{
	
}

#pragma region Init Functions
int Game::init()
{
	#if DEBUG
		
	#endif
	loadConfig();
	initSDL();
	initGlew();
	initObjects();
	time = 0;

	return 0;
}

int Game::initSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	Uint32 windowFlags;

	if (windowState == FULLSCREEN)		windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
	else if (windowState == WINDOWED)	windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
	else if (windowState == BORDERLESS) windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS;

	displayWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, windowFlags);
	
	displayContext = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, displayContext); 
	
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); 

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, bits);
	
    initOpenGL();
    resizeWindow(screenWidth, screenHeight);

	if (topDownCamera)	SDL_SetRelativeMouseMode(SDL_FALSE);
	else				SDL_SetRelativeMouseMode(SDL_TRUE);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    
	return 0;
}

int Game::initOpenGL()
{
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);  // clockwise oriented polys are front faces
	glCullFace(GL_BACK); // cull out the inner polygons... Set Culling property appropriately

	glShadeModel( GL_SMOOTH );							// Enable smooth shading
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );				// Set the background black
	glClearDepth( 1.0f );								// Depth buffer setup
	glEnable( GL_DEPTH_TEST );							// Enables Depth Testing
	glDepthFunc( GL_LEQUAL );							// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);			// filled polys

	buildFont();

	return 0;
}

GLvoid Game::buildFont()
{
	HFONT   font;                       // Windows Font ID
    HFONT   oldfont;                    // Used For Good House Keeping
 
    base = glGenLists(96);                  // Storage For 96 Characters ( NEW )

	font = CreateFont(  
		-24,						//Height Of Font ( NEW ) -indicates height instead of width
		0,							//Width Of Font
		0,							//Angle Of Escapement
		0,							//Orientation Angle
		FW_NORMAL,					//Font Weight
		FALSE,						//Italic
		FALSE,						//Underline
		FALSE,						//Strikeout
		OUT_TT_PRECIS,				// Output Precision
		ANSI_CHARSET,				//Character Set Identifier
		CLIP_DEFAULT_PRECIS,		//Clipping Precision
		ANTIALIASED_QUALITY,		//Output Quality
		FF_DONTCARE|DEFAULT_PITCH,	//Family And Pitch
		"Arial"); 				//Font Name

	HDC deviceContext = GetDC (NULL);

	oldfont = (HFONT)SelectObject(deviceContext, font);       // Selects The Font We Want
    wglUseFontBitmaps(deviceContext, 32, 96, base);           // Builds 96 Characters Starting At Character 32
    SelectObject(deviceContext, oldfont);             // Selects The Font We Want
    DeleteObject(font);                 // Delete The Font
}

int Game::initObjects()
{
	assetManager = new AssetManager();
	inputManager.init(&time);
	gameObjects = new list<GameObject*>();
	deadObjects = new list<GameObject*>();
	cardList = new list<Card*>();
	GUIList = new list<GUIItem*>();
	arenaObjects = new list<GameObject*>();
	GameObject::setStatics(&ignoreShaders, &view, &projection, gameObjects, assetManager);
	Card::setStatics(cardList, &playerID, &numPlayers, &showHand);
	GUIItem::setStatics(&camera, &inputManager, &screenWidth, &screenHeight, GUIList);
	playerID = 0;

	return 0;
}

int Game::initGlew()
{
	static bool alreadyExecuted = false;
	if (alreadyExecuted) return -1;
	alreadyExecuted = true;

	const bool loggingExtensions = true; //Set to true to have extensions logged...
	char *extensions = (char *) glGetString (GL_EXTENSIONS);

	//bool isARBVertexBufferObjectExtensionPresent = isExtensionSupported (extensions, "GL_ARB_vertex_buffer_object");
	//if (isARBVertexBufferObjectExtensionPresent) {
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress ("glBindBufferARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress ("glDeleteBuffersARB");
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress ("glGenBuffersARB");
		glIsBufferARB = (PFNGLISBUFFERARBPROC) wglGetProcAddress ("glIsBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress ("glBufferDataARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress ("glBufferSubDataARB");
		glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC) wglGetProcAddress ("glGetBufferSubDataARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress ("glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress ("glUnmapBufferARB");
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC) wglGetProcAddress ("glGetBufferParameterivARB");
		glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC) wglGetProcAddress ("glGetBufferPointervARB");
	//}
	
	//bool isGL_ARB_shader_objectsExtensionPresent = isExtensionSupported (extensions, "GL_ARB_shader_objects");
	//if (isGL_ARB_shader_objectsExtensionPresent) {
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress ("glAttachObjectARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress ("glCompileShaderARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress ("glCreateProgramObjectARB");
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress ("glCreateShaderObjectARB");
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress ("glDeleteObjectARB");
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress ("glDetachObjectARB");
		glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)wglGetProcAddress ("glGetActiveUniformARB");
		glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)wglGetProcAddress ("glGetAttachedObjectsARB");
		glGetHandleARB = (PFNGLGETHANDLEARBPROC)wglGetProcAddress ("glGetHandleARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress ("glGetInfoLogARB");
		glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)wglGetProcAddress ("glGetObjectParameterfvARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress ("glGetObjectParameterivARB");
		glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)wglGetProcAddress ("glGetShaderSourceARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress ("glGetUniformLocationARB");
		glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)wglGetProcAddress ("glGetUniformfvARB");
		glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)wglGetProcAddress ("glGetUniformivARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress ("glLinkProgramARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress ("glShaderSourceARB");
		glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress ("glUniform1fARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress ("glUniform1fvARB");
		glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress ("glUniform1iARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)wglGetProcAddress ("glUniform1ivARB");
		glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress ("glUniform2fARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress ("glUniform2fvARB");
		glUniform2iARB = (PFNGLUNIFORM2IARBPROC)wglGetProcAddress ("glUniform2iARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)wglGetProcAddress ("glUniform2ivARB");
		glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress ("glUniform3fARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress ("glUniform3fvARB");
		glUniform3iARB = (PFNGLUNIFORM3IARBPROC)wglGetProcAddress ("glUniform3iARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)wglGetProcAddress ("glUniform3ivARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress ("glUniform4fARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress ("glUniform4fvARB");
		glUniform4iARB = (PFNGLUNIFORM4IARBPROC)wglGetProcAddress ("glUniform4iARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)wglGetProcAddress ("glUniform4ivARB");
		glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)wglGetProcAddress ("glUniformMatrix2fvARB");
		glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress ("glUniformMatrix3fvARB");
		glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress ("glUniformMatrix4fvARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress ("glUseProgramObjectARB");
		glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)wglGetProcAddress ("glValidateProgramARB");

		// Other Shader Stuff
		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress ("glCompileShader");
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress ("glCreateProgram");
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress ("glCreateShader");
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress ("glDeleteProgram");
		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress ("glDeleteShader");
		glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress ("glDetachShader");
		glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress ("glGetAttachedShaders");
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress ("glGetUniformLocation");
		glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress ("glUniform1f");
		glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress ("glUniform2f");
		glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress ("glUniform3f");
		glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress ("glUniform4f");
		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress ("glUniform1i");		
	//}
	glewInit();

	return 0;
}
#pragma endregion

#pragma region Save/Load Functions
void Game::loadConfig()
{
	//Default Settings
	windowState = BORDERLESS;
	bits = 32;
	screenWidth = 1280;
	screenHeight = 720;
	maxFps = 120;
	topDownZoom = 1.2f;

	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, false};
	CreateDirectory("Config", &sa);

	FILE * file;
	char * configName = "Config\\settings.ini";
	file = fopen(configName, "r");
	if (file == NULL) return; //no file found, just use defaults

	ifstream fileStream(configName);
	string s;
	string var, val;
	while (fileStream.peek() != -1)
	{
		s = var = val = "";
		getline(fileStream, s);
		bool eq = false;
		int i = 0;

		while (i < s.size())
		{
			if (s[i] == '=')	eq = true;
			else if (eq)		val += s[i];
			else				var += s[i];
			i++;
		}

		if (var == "maxFps")			maxFps = atoi(val.c_str());
		else if (var == "screenWidth")	screenWidth = atoi(val.c_str());
		else if (var == "screenHeight")	screenHeight = atoi(val.c_str());
		else if (var == "windowState")	windowState = atoi(val.c_str());
		else if (var == "topDown")		topDownCamera = (strcmp(val.c_str(), "true") == 0);
	}
	
	//fclose(file);
}

void Game::saveConfig()
{
	ofstream output;
	char * configName = "Config\\settings.ini";
	output.open(configName);
	
	
	const unsigned char * ver = glGetString(GL_VERSION);
	string v = "";
	int i = 0;
	while (ver[i] != ' ') { v += ver[i]; i++; }

	float version = atof(v.c_str());

	output 
		<< "maxFps=" << maxFps << '\n'
		<< "screenWidth=" << screenWidth << '\n'
		<< "screenHeight=" << screenHeight << '\n'
		<< "windowState=" << windowState << '\n'
		<< "topDown=" << (topDownCamera ? "true" : "false") << '\n'
		<< "openGLVersion=" << version;

	output.close();
}

int Game::loadLevel(string input)
{
	arenaObjects->clear();
	while (gameObjects->size() > 0) {
		delete gameObjects->front();
		gameObjects->pop_front();
	}

	if (input == "Main Menu") {
		GUIItem * temp = new GUIItem("Textures\\title.png", "Shaders\\default.glsl");
		temp->setDimensions(screenWidth, screenHeight);
		temp->setScreenPosition(screenWidth/2.0f, screenHeight/2.0f);
	}
	else {
		Card::initDecks(numPlayers);
		if (deck != NULL) delete [] deck;
		if (graveyard != NULL) delete [] graveyard;
		if (exile != NULL) delete [] exile;
		deck = new list<string>[numPlayers];
		graveyard = new list<string>[numPlayers];
		exile = new list<string>[numPlayers];

		for (int i = 0; i < numPlayers; i++) {
			deck[i] = list<string>();
			loadDeck("test", &deck[i]);
			graveyard[i] = list<string>();
			exile[i] = list<string>();

			Card * temp = new Card("Textures\\cardback.jpg", i);
			temp->setRegion(region_deck);
			temp = new Card("Textures\\graveyard.jpg", i);
			temp->setPosition(temp->getGraveyardPosition());
			temp->setTargetPosition(temp->getGraveyardPosition());
			temp->setRegion(region_graveyard);
			temp->forceFlip();
			temp = new Card("Textures\\exile.jpg", i);
			temp->setPosition(temp->getExilePosition());
			temp->setRegion(region_exile);
			temp->setTargetPosition(temp->getExilePosition());
			temp->forceFlip();

			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\longregion.png", "Shaders\\default.glsl", 
				-temp->getDeckDirection() * combatZ - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(deckX*2-1.0-sectionSpacing, 1.0f+sectionSpacing, 1.0f)));
			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\longregion.png", "Shaders\\default.glsl", 
				-temp->getDeckDirection() * permanentZ - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(deckX*2-1.0-sectionSpacing, 1.0f+sectionSpacing, 1.0f)));
			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\longregion.png", "Shaders\\default.glsl", 
				-temp->getDeckDirection() * landZ - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(deckX*2-1.0-sectionSpacing, 1.0f+sectionSpacing, 1.0f)));
			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\smallregion.png", "Shaders\\default.glsl", 
				-temp->getDeckPosition() - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(1.0, 1.0f+sectionSpacing, 1.0f)));
			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\smallregion.png", "Shaders\\default.glsl", 
				-temp->getGraveyardPosition() - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(1.0, 1.0f+sectionSpacing, 1.0f)));
			arenaObjects->push_back(new GameObject("quad.obj", "Textures\\smallregion.png", "Shaders\\default.glsl", 
				-temp->getExilePosition() - glm::vec3(0,0.1f,0), glm::vec3(0,1,0), temp->getDeckDirection(), glm::vec3(1.0, 1.0f+sectionSpacing, 1.0f)));

			playerID = i;
			shuffleDeck();
		}
	}

	playerID = 0;

	return 0;
}

int Game::loadDeck(string deckName, list<string> * deck)
{
	if (deck == NULL) return -1;

	FILE * file;
	string deckPath = "Decks\\" + deckName + ".deck";
	file = fopen(deckPath.c_str(), "r");
	if (file == NULL) return -1; //no file found, just use defaults

	ifstream fileStream(deckPath);
	string s;
	string var, val;
	while (fileStream.peek() != -1)
	{
		s = var = val = "";
		getline(fileStream, s);
		bool eq = false;
		int i = 0;

		while (i < s.size())
		{
			if (eq == false && s[i] == ':')	eq = true;
			else if (eq)		var += s[i];
			else				val += s[i];
			i++;
		}

		for (int i = 0; i < atoi(val.c_str()); i++) deck->push_back("Textures\\Cards\\" + var);
	}
	
	return 0;
}
#pragma endregion

#pragma region Cleanup Functions
int Game::cleanupObjects()
{
	if (deck != NULL) delete [] deck;
	if (graveyard != NULL) delete [] graveyard;
	if (exile != NULL) delete [] exile;

	while (gameObjects->size() > 0) {
		delete gameObjects->front();
		gameObjects->pop_front();
	}

	delete gameObjects;
	deadObjects->clear();
	delete deadObjects;
	delete cardList;
	delete GUIList;
	delete arenaObjects;

	assetManager->shutdown();
	SDL_WaitThread(assetLoaderThread, NULL);
	delete assetManager;
	return 0;
}

int Game::killSDL() 
{
	IMG_Quit();
	SDL_GL_DeleteContext(displayContext);
    SDL_DestroyWindow(displayWindow);
	SDL_Quit();

	return 0;
}

GLvoid Game::killFont()
{
    glDeleteLists(base, 96);                // Delete All 96 Characters ( NEW )
}

int Game::cleanup()
{
	cleanupObjects();
	defaultShader = 0;
	quad = 0;
	startScreen = 0;

	saveConfig();
	
	killSDL();
	killFont();

	//#if DEBUG
	//	_CrtDumpMemoryLeaks();
	//#endif

	PostQuitMessage(0);
	return 0;
}
#pragma endregion

#pragma region Thread Functions
int Game::start()
{
	double elapsedTime = 0.0;
	double timePerFrame = 1000.0 / maxFps;
	long lastTime = 0;
	
	double targetSleep = 0.0;
	double sleepRemainder = 0.0;
	double startSleep = 0.0;
	double sleepTime = 0.0;
	
	assetManager->forceLoadModel("quad.obj");
	quad = assetManager->getModel("quad.obj");
	assetManager->forceLoadModel("card.obj");
	card = assetManager->getModel("card.obj");
	assetManager->forceLoadTexture("Textures\\cardback.jpg");
	cardBack = assetManager->getTexture("Textures\\cardback.jpg");
	assetManager->forceLoadTexture("Textures\\starttemp.tga");
	startScreen = assetManager->getTexture("Textures\\starttemp.tga");
	assetManager->forceLoadShader("Shaders\\default.glsl");
	defaultShader = assetManager->getShader("Shaders\\default.glsl");
	defaultShader->activate();
	assetManager->forceLoadShader("Shaders\\text.glsl");
	textShader = assetManager->getShader("Shaders\\text.glsl");

	assetManager->forceLoadTexture("Textures\\black.png");
	assetManager->forceLoadTexture("Textures\\white.png");
	assetManager->forceLoadTexture("Textures\\blue.png");
	assetManager->forceLoadTexture("Textures\\green.png");
	assetManager->forceLoadTexture("Textures\\red.png");
	assetManager->forceLoadTexture("Textures\\inf.png");
	assetManager->forceLoadTexture("Textures\\1.png");
	assetManager->forceLoadTexture("Textures\\X.png");
	assetManager->forceLoadTexture("Textures\\bp.png");
	assetManager->forceLoadTexture("Textures\\s.png");
	cursorTexture = assetManager->getTexture("Textures\\black.png");

	loadLevel("Main Menu");
		
	while(running)
	{
		time++;
		lastTime = SDL_GetTicks();

		inputManager.clearTempValues();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)	return 1;
			else if (e.type == SDL_MOUSEBUTTONDOWN) inputManager.mouseKeyDown(e.button.button);
			else if (e.type == SDL_MOUSEBUTTONUP)	inputManager.mouseKeyUp(e.button.button);
			else if (e.type == SDL_KEYDOWN)			inputManager.keyDown(e.key.keysym.sym);
			else if (e.type == SDL_KEYUP)			inputManager.keyUp(e.key.keysym.sym);
			else if (e.type == SDL_MOUSEMOTION)		inputManager.updateMouse(e.motion.xrel, e.motion.yrel, e.motion.x, e.motion.y); 
			else if (e.type == SDL_MOUSEWHEEL)		{ inputManager.updateScroll(e.wheel.y); }
			else {}
		}
		
		if (update(elapsedTime+sleepTime) == -1) break;
		renderFrame(SDL_GetTicks());
		
		double averageElapsedTime = calcFps(elapsedTime+sleepTime);
		frameRate = 1000.0/averageElapsedTime;

		//Framerate Limit Calculations
		elapsedTime = SDL_GetTicks() - lastTime;
		targetSleep = timePerFrame - elapsedTime + sleepRemainder;
		if (targetSleep > 0) sleepRemainder = targetSleep - (Uint32)targetSleep;

		startSleep = SDL_GetTicks();
		while ((Uint32)(startSleep+targetSleep) > SDL_GetTicks());
		sleepTime = SDL_GetTicks() - startSleep;
		glFinish();
	}
    return 0;
}

int Game::startAssetLoader(void * data)
{
	assetManager->startAssetLoader();
	return 0;
}

int Game::startHostThread(void * data)
{

	return 0;
}

int Game::startClientThread(void * data)
{

	return 0;
}
#pragma endregion

#pragma region Utility Functions
int Game::resizeWindow(int width, int height) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	int horizontal = desktop.right;
	int vertical = desktop.bottom;
	if (horizontal == width && vertical == height)
		SDL_SetWindowSize(displayWindow, width+1, height+1); 
	else
		SDL_SetWindowSize(displayWindow, width, height); 

	//Setup a new viewport.
	glViewport (0, 0, width, height);
	screenWidth = width; screenHeight = height;

	//Setup a new perspective matrix.
	GLdouble verticalFieldOfViewInDegrees = 40;
	GLdouble aspectRatio = height == 0 ? 1.0 : (GLdouble) width / (GLdouble) height;
	camera.setAspectRatio(aspectRatio);
	GLdouble nearDistance = 1.0;
	GLdouble farDistance = 2000.0;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (verticalFieldOfViewInDegrees, aspectRatio, nearDistance, farDistance);

	//Get back to default mode.
	glMatrixMode (GL_MODELVIEW);

	//9 minimum
	if (aspectRatio > 1.0f) { //X is larger
		topDownY = 11.0;
		topDownX = 11.0*aspectRatio;
	}
	else { //y is larger
		topDownX = 11.0f;
		topDownY = 11.0/aspectRatio;
	}

	return 0;
}

double Game::calcFps(double newtick)
{
	ticksum-=ticklist[tickindex];  /* subtract value falling off */
    ticksum+=newtick;              /* add new value */
    ticklist[tickindex]=newtick;   /* save new value so it can be subtracted later */
    if(++tickindex==MAXFRAMESAMPLES)    /* inc buffer index */
        tickindex=0;

    /* return average */
    return((ticksum)/MAXFRAMESAMPLES);
}

POINT Game::getScreenSize() 
{
	POINT p;
	return p;
}

DWORD Game::getRefreshRate()
{
	DEVMODE *lpDevMode= new DEVMODE();
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, lpDevMode) == 0) 
		return 60;
	DWORD result = lpDevMode->dmDisplayFrequency;
	delete lpDevMode;
	return result;
}
#pragma endregion

#pragma region Update
int Game::updateSelectedCard() {
	list<Card *> planeIntersectList;
	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		(*it)->setSelected(false);
		glm::vec3 p0 = (*it)->getPosition();
		glm::vec3 n = (*it)->getDirection();
	
		glm::vec3 l0 = camera.getPos();
		glm::vec3 l = camera.getLookAtVector();

		if (topDownCamera) {
			float px = (float)inputManager.getMousePosition().x/(float)screenWidth;
			float py = (float)inputManager.getMousePosition().y/(float)screenHeight;
			l0 = glm::vec3(-(px-0.5)*topDownX*topDownZoom, 10, -(py-0.5)*topDownY*topDownZoom);
			l = glm::vec3(0, -1, 0);
		}
		//(float)20, (float)11.25f

		// assuming vectors are all normalized
		float denom = glm::dot(n, l);
		float t = 0;
		glm::vec3 p0l0 = p0 - l0;
		t = glm::dot(p0l0, n) / denom; 
		if (t >= 0) {
			glm::vec3 p = l0 + l*t;
			glm::vec3 vec = p - p0;
			glm::vec3 up = (*it)->getUp();
			glm::vec3 side = glm::normalize(glm::cross((*it)->getDirection(), (*it)->getUp()));
			glm::vec3 upProj = up * glm::dot(vec, up) / Utilities::magnitude(up) / Utilities::magnitude(up);
			glm::vec3 sideProj = side * glm::dot(vec, side) / Utilities::magnitude(side) / Utilities::magnitude(side);

			if (Utilities::magnitude(upProj) < 1.0f/2.0f &&
				Utilities::magnitude(sideProj) < 12.0f/17.0f/2.0f)
				planeIntersectList.push_back(*it);
		}
	}

	int index = -1;
	int i = 0;
	float dist = 100.0f;
	for (list<Card*>::iterator it = planeIntersectList.begin(); it != planeIntersectList.end(); it++) {
		glm::vec3 aboveObject = glm::vec3((*it)->getPosition().x, 5.0f, (*it)->getPosition().z);
		if (Utilities::magnitude(aboveObject - (*it)->getPosition()) < dist) {
			dist = Utilities::magnitude(glm::vec3(aboveObject - (*it)->getPosition()));
			index = i;
		}
		i++;
	}

	selectedCard = 0;
	i = 0;
	for (list<Card*>::iterator it = planeIntersectList.begin(); it != planeIntersectList.end(); it++) {
		if (index == i) {
			(*it)->setSelected(true);
			selectedCard = (*it);
		}
		i++;
	}

	return 0;
}

int Game::untapAll() {
	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		(*it)->unTap();
	}
	return 0;
}

void Game::updateGraveyardTexture() 
{
	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		if ((*it)->getCardRegion() == region_graveyard && (*it)->getOwnerIndex() == playerID) {
			if (graveyard[playerID].size() == 0)
				(*it)->changeTexture("Textures\\graveyard.jpg");
			else
				(*it)->changeTexture(graveyard[playerID].front());
		}
	}
}

void Game::updateExileTexture()
{
	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		if ((*it)->getCardRegion() == region_exile && (*it)->getOwnerIndex() == playerID) {
			if (exile[playerID].size() == 0)
				(*it)->changeTexture("Textures\\exile.jpg");
			else
				(*it)->changeTexture(exile[playerID].front());
		}
	}
}

int Game::updatePlayerCards(int PlayerID)
{
	list<Card*> deckList;
	list<Card*> handList;
	list<Card*> landList;
	list<Card*> permanentList;
	list<Card*> combatList;
	list<Card*> graveyardList;
	list<Card*> graveyardViewList;
	list<Card*> exileList;
	list<Card*> exileViewList;

	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		if ((*it)->getOwnerIndex() != PlayerID) continue;
		if ((*it)->getCardRegion() == region_topDeck || (*it)->getCardRegion() == region_bottomDeck)
			deckList.push_back(*it);
		else if ((*it)->getCardRegion() == region_hand)
			handList.push_back(*it);
		else if ((*it)->getCardRegion() == region_land)
			landList.push_back(*it);
		else if ((*it)->getCardRegion() == region_permanent)
			permanentList.push_back(*it);
		else if ((*it)->getCardRegion() == region_combat)
			combatList.push_back(*it);
		else if ((*it)->getCardRegion() == region_topGraveyard)
			graveyardList.push_back(*it);
		else if ((*it)->getCardRegion() == region_graveyardView)
			graveyardViewList.push_back(*it); 
		else if ((*it)->getCardRegion() == region_topExile)
			exileList.push_back(*it);
		else if ((*it)->getCardRegion() == region_exileView)
			exileViewList.push_back(*it);
	}

	for (list<Card*>::iterator it = deckList.begin(); it != deckList.end(); it++) {
		if ((*it)->getPosition() == (*it)->getDeckPosition()) {
			(*it)->setAlive(false);
			if ((*it)->getCardRegion() == region_topDeck)
				deck[playerID].push_front((*it)->getTexturePath());
			if ((*it)->getCardRegion() == region_bottomDeck)
				deck[playerID].push_back((*it)->getTexturePath());
		}
		(*it)->setTargetPosition((*it)->getDeckPosition());
	}

	int i = 0;
	for (list<Card*>::iterator it = handList.begin(); it != handList.end(); it++) {
		if (showHand)
			(*it)->setTargetPosition(-(*it)->getDeckDirection() * handZ + 
				(*it)->getTapDirection() * (float)(i - (handList.size()-1)/2.0f) + glm::vec3(0,1,0));
		else
			(*it)->setTargetPosition(-(*it)->getDeckDirection() * (handZ+4) + 
				(*it)->getTapDirection() * (float)(i - (handList.size()-1)/2.0f) + glm::vec3(0,1,0));
		i++;
	}

	list<list<Card*>> uniqueLandList;
	for (list<Card*>::iterator it = landList.begin(); it != landList.end(); it++) {	
		bool recurringCard = false;
		for (list<list<Card*>>::iterator it2 = uniqueLandList.begin(); it2 != uniqueLandList.end(); it2++) {
			if ((*it2).front()->getCardName() == (*it)->getCardName()) {
				(*it2).push_back(*it);
				recurringCard = true;
			}
		}
		if (recurringCard == false) {
			uniqueLandList.push_back(list<Card*>());
			uniqueLandList.back().push_front(*it);
		}
	}

	i = 0;
	for (list<list<Card*>>::iterator it = uniqueLandList.begin(); it != uniqueLandList.end(); it++) {
		int stacki = 0;
		for (list<Card*>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
			(*it2)->setTargetPosition(-(*it2)->getDeckDirection()*(float)(landZ+stacki*0.2f) + (*it2)->getTapDirection() * (float)(i-(uniqueLandList.size()-1)/2.0f) + glm::vec3(0,0.025f,0)*(float)stacki);
			stacki++;
		}
		i++;
	}

	i = 0;
	for (list<Card*>::iterator it = permanentList.begin(); it != permanentList.end(); it++) {
		(*it)->setTargetPosition(-(*it)->getDeckDirection() * permanentZ + 
			(*it)->getTapDirection() * (float)(i - (permanentList.size()-1)/2.0f));
		i++;
		//need to stack equips and shit?
	}
	
	i = 0;
	for (list<Card*>::iterator it = combatList.begin(); it != combatList.end(); it++) {
		(*it)->setTargetPosition(-(*it)->getDeckDirection() * combatZ + 
			(*it)->getTapDirection() * (float)(i - (combatList.size()-1)/2.0f));
		i++;
	}

	i = 0;
	for (list<Card*>::iterator it = graveyardList.begin(); it != graveyardList.end(); it++) {
		if ((*it)->getPosition() == (*it)->getGraveyardPosition()) {
			(*it)->setAlive(false);
			graveyard[playerID].push_front((*it)->getTexturePath());
			updateGraveyardTexture();
		}
		else {
			(*it)->setTargetPosition((*it)->getGraveyardPosition());
		}
		i++;
	}

	i = 0;
	for (list<Card*>::iterator it = graveyardViewList.begin(); it != graveyardViewList.end(); it++) {
		(*it)->setTargetPosition((*it)->getGraveyardPosition() + (*it)->getDeckDirection() * (float)(i+1+(*it)->getGraveyardOffset()));
		i++;
	}

	i = 0;
	for (list<Card*>::iterator it = exileList.begin(); it != exileList.end(); it++) {
		if ((*it)->getPosition() == (*it)->getExilePosition()) {
			(*it)->setAlive(false);
			exile[playerID].push_front((*it)->getTexturePath());
			updateExileTexture();
		}
		else {
			(*it)->setTargetPosition((*it)->getExilePosition());
		}
		i++;
	}

	i = 0;
	for (list<Card*>::iterator it = exileViewList.begin(); it != exileViewList.end(); it++) {
		(*it)->setTargetPosition((*it)->getExilePosition() + (*it)->getDeckDirection() * (float)(i+1+(*it)->getExileOffset()));
		i++;
	}

	return 0;
}

int Game::updateCards()
{
	for (int i = 0; i < numPlayers; i++)
		updatePlayerCards(i);

	if (isKeyPressed(IM_Q)) untapAll();

	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
		if ((*it)->getOwnerIndex() == playerID && (*it)->isSelected()) {
			if ((*it)->getCardRegion() == region_deck) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					if (deck[playerID].size() > 0) {
						Card * temp = new Card(deck[playerID].front(), playerID);
						deck[playerID].pop_front();
						temp->moveToHand();
					}
				}
			}
			else if ((*it)->getCardRegion() == region_topDeck || (*it)->getCardRegion() == region_bottomDeck) {

			}
			else if ((*it)->getCardRegion() == region_hand) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					if (Card::isLand((*it)->getTexturePath()))
						(*it)->moveToLands();
					else
						(*it)->moveToPermanents();
				}
			}
			else if ((*it)->getCardRegion() == region_land) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					(*it)->toggleTap();
				}
			}
			else if ((*it)->getCardRegion() == region_permanent) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					(*it)->toggleTap();
				}
			}
			else if ((*it)->getCardRegion() == region_combat) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					(*it)->toggleTap();
				}
			}
			else if ((*it)->getCardRegion() == region_graveyard) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					bool returnedCards = false;
					for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
						if ((*it)->getOwnerIndex() == playerID && (*it)->getCardRegion() == region_graveyardView) {
							returnedCards = true;
							(*it)->setRegion(region_topGraveyard);
						}
					}

					if (returnedCards == false) {
						while (graveyard[playerID].size() > 0) {
							Card * temp = new Card(graveyard[playerID].front(), playerID);
							temp->setPosition(temp->getGraveyardPosition()+glm::vec3(-0.05f,0,0));
							temp->forceFlip();
							temp->setDirection(glm::vec3(0,1,0));
							temp->setRegion(region_graveyardView);
							
							graveyard[playerID].pop_front();
							updateGraveyardTexture();
						}
					}
				}
			}
			else if ((*it)->getCardRegion() == region_exile) {
				if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) < maxFps/holdDivisor) {
					bool returnedCards = false;
					for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
						if ((*it)->getOwnerIndex() == playerID && (*it)->getCardRegion() == region_exileView) {
							returnedCards = true;
							(*it)->setRegion(region_topExile);
						}
					}

					if (returnedCards == false) {
						while (exile[playerID].size() > 0) {
							Card * temp = new Card(exile[playerID].front(), playerID);
							temp->setPosition(temp->getExilePosition()+glm::vec3(-0.05f,0,0));
							temp->forceFlip();
							temp->setDirection(glm::vec3(0,1,0));
							temp->setRegion(region_exileView);
							
							exile[playerID].pop_front();
							updateExileTexture();
						}
					}
				}
			}
		}
	}
	return 0;
}

#pragma region Menu Functions
void Game::openClickMenu()
{
	GUIItem * temp = new GUIItem("Textures\\ClickMenu.png", "Shaders\\menu.glsl"); 
	temp->setDimensions(yWidth*6, yWidth*3);
	temp->addRegion(0, yWidth*0, yWidth*6, yWidth*1, &Game::openActionsMenu);
	temp->addRegion(0, yWidth*1, yWidth*6, yWidth*2, &Game::openMoveMenu);
	temp->addRegion(0, yWidth*2, yWidth*6, yWidth*3, &Game::openTokensMenu);
	temp->setScreenPosition((float)inputManager.getMousePosition().x, (float)inputManager.getMousePosition().y);
}

void Game::openActionsMenu()
{
	GUIItem * temp = new GUIItem("Textures\\ActionsMenu.png", "Shaders\\menu.glsl"); 
	temp->setDimensions(yWidth*6, yWidth*4);
	temp->addRegion(0, yWidth*0, yWidth*6, yWidth*1, &Game::openCountersMenu);
	temp->addRegion(0, yWidth*1, yWidth*6, yWidth*2, &Game::flipSelected);
	temp->addRegion(0, yWidth*2, yWidth*6, yWidth*3, &Game::tapSelected);
	temp->addRegion(0, yWidth*3, yWidth*6, yWidth*4, &Game::shuffleDeck);
	temp->setScreenPosition((float)inputManager.getMousePosition().x, (float)inputManager.getMousePosition().y);
}

void Game::openMoveMenu()
{
	GUIItem * temp = new GUIItem("Textures\\MoveMenu.png", "Shaders\\menu.glsl"); 
	temp->setDimensions(yWidth*6, yWidth*8);
	temp->addRegion(0, yWidth*0, yWidth*6, yWidth*1, &Game::moveToAttackers);
	temp->addRegion(0, yWidth*1, yWidth*6, yWidth*2, &Game::moveToTopDeck);
	temp->addRegion(0, yWidth*2, yWidth*6, yWidth*3, &Game::moveToBottomDeck);
	temp->addRegion(0, yWidth*3, yWidth*6, yWidth*4, &Game::moveToHand);
	temp->addRegion(0, yWidth*4, yWidth*6, yWidth*5, &Game::moveToLands);
	temp->addRegion(0, yWidth*5, yWidth*6, yWidth*6, &Game::moveToPermanents);
	temp->addRegion(0, yWidth*6, yWidth*6, yWidth*7, &Game::moveToGraveyard);
	temp->addRegion(0, yWidth*7, yWidth*6, yWidth*8, &Game::moveToExile);

	temp->setScreenPosition((float)inputManager.getMousePosition().x, (float)inputManager.getMousePosition().y);
}

void Game::openTokensMenu()
{

}

void Game::openCountersMenu()
{

}

void Game::flipSelected()	{ if (selectedCard != NULL) selectedCard->flip(); }
void Game::tapSelected()	{ if (selectedCard != NULL) selectedCard->toggleTap(); }
void Game::shuffleDeck()
{
	vector<string> tempDeck;
	while (deck[playerID].size() > 0) {
		tempDeck.push_back(deck[playerID].front());
		deck[playerID].pop_front();
	}

	for (int i = 0; i < 1; i++)
		std::random_shuffle(tempDeck.begin(), tempDeck.end());
	
	while (tempDeck.size() > 0) {
		deck[playerID].push_front(tempDeck.back());
		tempDeck.pop_back();
	}

	for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++) {
			if ((*it)->getCardRegion() == region_deck && (*it)->getOwnerIndex() == playerID) {
				(*it)->flipX(1.0f/maxFps*180.0f/M_PI*15.0f);
			}
	}
}
void Game::moveToAttackers()	{ if (selectedCard != NULL) selectedCard->moveToCombat(); }
void Game::moveToTopDeck()		{ if (selectedCard != NULL) selectedCard->moveToTopDeck(); }
void Game::moveToBottomDeck()	{ if (selectedCard != NULL) selectedCard->moveToBottomDeck(); }
void Game::moveToHand()			{ if (selectedCard != NULL) selectedCard->moveToHand(); }
void Game::moveToLands()		{ if (selectedCard != NULL) selectedCard->moveToLands(); }
void Game::moveToPermanents()	{ if (selectedCard != NULL) selectedCard->moveToPermanents(); }
void Game::moveToGraveyard()	{ if (selectedCard != NULL) selectedCard->moveToGraveyard(); }
void Game::moveToExile()		{ if (selectedCard != NULL) selectedCard->moveToExile(); }

#pragma endregion

int Game::update(long elapsedTime)
{
	if (isKeyPressed(IM_ESCAPE)) return -1;

	if (state == 0)	{
		if (isKeyPressed(IM_SPACE)) {
			state = 1;
			loadLevel("ASDF");
		}
	}
	else if (state == 1) {

		if (isKeyDown(IM_W))		camera.moveForward(elapsedTime*0.005);
		if (isKeyDown(IM_S))		camera.moveBack(elapsedTime*0.005);
		if (isKeyDown(IM_A))		camera.moveLeft(elapsedTime*0.005);
		if (isKeyDown(IM_D))		camera.moveRight(elapsedTime*0.005);
		//if (isKeyDown(IM_SPACE))	camera.moveUp(elapsedTime*0.005);
		//if (isKeyDown(IM_LCTRL))	camera.moveDown(elapsedTime*0.005);

		if (isKeyPressed(IM_1)) cursorTexture = assetManager->getTexture("Textures\\black.png");
		if (isKeyPressed(IM_2)) cursorTexture = assetManager->getTexture("Textures\\blue.png");
		if (isKeyPressed(IM_3)) cursorTexture = assetManager->getTexture("Textures\\green.png");
		if (isKeyPressed(IM_4)) cursorTexture = assetManager->getTexture("Textures\\red.png");
		if (isKeyPressed(IM_5)) cursorTexture = assetManager->getTexture("Textures\\white.png");
		if (isKeyPressed(IM_6)) cursorTexture = assetManager->getTexture("Textures\\inf.png");
		if (isKeyPressed(IM_7)) cursorTexture = assetManager->getTexture("Textures\\1.png");
		if (isKeyPressed(IM_8)) cursorTexture = assetManager->getTexture("Textures\\X.png");
		if (isKeyPressed(IM_9)) cursorTexture = assetManager->getTexture("Textures\\bp.png");
		if (isKeyPressed(IM_0)) cursorTexture = assetManager->getTexture("Textures\\s.png");

		/*if (topDownCamera) {
			if (isMouseScrollUp()) { topDownZoom -= 0.1f; if (topDownZoom < 0.5f) topDownZoom = 0.5f; }
			if (isMouseScrollDown()) { topDownZoom += 0.1f; if (topDownZoom > 5.0f) topDownZoom = 5.0f; }
		}*/

		if (isKeyReleased(IM_M1) && inputManager.getHoldDuration(IM_M1) > maxFps/holdDivisor && heldCard != NULL) {
			POINT p = Card::getPointRegion(glm::vec2(heldCard->getPosition().x, heldCard->getPosition().z));
			if (p.x != -1) heldCard->setRegion(p.x);
			if (p.y != -1) heldCard->setOwnerIndex(p.y);
			heldCard = NULL;
		}

		if (isKeyPressed(IM_M1) && selectedCard != NULL && selectedCard->getOwnerIndex() == playerID) {
			if (selectedCard->getCardRegion() != region_deck && selectedCard->getCardRegion() != region_graveyard && selectedCard->getCardRegion() != region_exile) {
				heldCard = selectedCard;
			}
		}

		if (isKeyDown(IM_M1) && heldCard != NULL && (time - inputManager.getLastDown(IM_M1)) > maxFps/(holdDivisor+2)) {
			if (topDownCamera) {
				glm::vec2 p2d = Utilities::screenToWorldPoint(inputManager.getMousePosition(), screenWidth, screenHeight, topDownX*topDownZoom, topDownY*topDownZoom);
				heldCard->setPosition(p2d.x, 1.1f, p2d.y);
				heldCard->setTargetPosition(p2d.x, 1.1f, p2d.y);
			}
			else {
				glm::vec3 p3d = Utilities::perspectiveToWorldPos(camera.getPos(), camera.getLookAtVector());
				heldCard->setPosition(p3d);
				heldCard->setTargetPosition(p3d);
			}
		}

		if (topDownCamera) {
			if (isKeyDown(IM_M2)) {
				char buffer[2];
				buffer[0] = '0'+(rand()%4+1);
				buffer[1] = '\0';
				Card * temp = new Card("Textures\\Cards\\M15\\swamp" + string(buffer) + ".hq.jpg", playerID);
				glm::vec2 worldPos = Utilities::screenToWorldPoint(inputManager.getMousePosition(), screenWidth, screenHeight, topDownX*topDownZoom, topDownY*topDownZoom);
				temp->setPosition(worldPos.x, 10, worldPos.y);
				temp->moveToHand();
			}
			if (isKeyPressed(IM_M3)) {
				openClickMenu();
			}
		}

		if (isMouseScrollUp()) {
			if (selectedCard != NULL) {
				if (selectedCard->getCardRegion() == region_graveyard || selectedCard->getCardRegion() == region_graveyardView) 
					Card::incGraveyardOffset(); 
				else if (selectedCard->getCardRegion() == region_exile || selectedCard->getCardRegion() == region_exileView) 
					Card::incExileOffset(); 
			}
		}
		if (isMouseScrollDown()) { 
			if (selectedCard != NULL) {
				if (selectedCard->getCardRegion() == region_graveyard || selectedCard->getCardRegion() == region_graveyardView) 
					Card::decGraveyardOffset(); 
				else if (selectedCard->getCardRegion() == region_exile || selectedCard->getCardRegion() == region_exileView) 
					Card::decExileOffset(); 
			}
		}


		if (isKeyPressed(IM_LCTRL)) showHand = !showHand;
		renderArena = isKeyDown(IM_TAB);

		if (isKeyPressed(IM_P)) { 
			topDownCamera = !topDownCamera;
			if (!topDownCamera) {
				camera.setUpVector(0,1,0);
				camera.setPosition(0, 6, -12);
				camera.setLookAtPoint(0,0,0); 
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
			else SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		if (isKeyPressed(IM_U)) playerID = (playerID+1)%numPlayers;

		camera.update(elapsedTime);
		camera.turn(inputManager.getMouseMovement());

		if (isKeyPressed(IM_O)) 
		{
			loadLevel("ASDF");
		}
	}

	for (list<GameObject*>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
		(*it)->update(elapsedTime);

	int i = 0;
	for (list<GUIItem*>::reverse_iterator it = GUIList->rbegin(); it != GUIList->rend(); it++)
		i = (*it)->updateGUI(elapsedTime, i);
	
	if (i == 0) { 
		updateSelectedCard();
		updateCards();
	}
		
	deadObjects->clear();
	for (list<GameObject*>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++) {
		if (!(*it)->isAlive())
			deadObjects->push_back(*it);
	}

	while (deadObjects->size() > 0) {
		gameObjects->remove(deadObjects->front());
		delete deadObjects->front();
		deadObjects->pop_front();
	}
	
	return 0;
}
#pragma endregion

#pragma region Render
int Game::renderFrame(long totalElapsedTime) {
	glClearColor (0.3, 0.3, 0.8, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
		
	render3D(totalElapsedTime);
	render2D(totalElapsedTime);
	
	SDL_GL_SwapWindow(displayWindow);
	return 0;
}

int Game::renderTooltipCard(long totalElapsedTime)
{
	if (selectedCard != NULL && selectedCard->getTexture() != NULL) {
		Texture * t =  selectedCard->getTexture();
		if (topDownCamera) {
			float px = selectedCard->getPosition().x;
			float py = selectedCard->getPosition().z;
			if (py < 0) 
				drawCard(glm::vec3(0, 1, topDownY/4.5f), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), t, glm::vec3(6, 6, 1)); 
			else
				drawCard(glm::vec3(0, 1, -topDownY/4.5f), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), t, glm::vec3(6, 6, 1)); 
		}
		else {
			glDisable(GL_DEPTH_TEST);
			drawCard(camera.getPos()+camera.getLookAtVector()*2.0f, -camera.getLookAtVector(), camera.getUpVector(), t, glm::vec3(1, 1, 1)); 
			glEnable(GL_DEPTH_TEST);
		}
		return 0;
	}
	return -1;
}

void Game::drawCard(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Texture * t, glm::vec3 size) {
	glm::mat4 world, scale;
	world = glm::inverse((glm::lookAt(position, position+direction, up)));
	scale = glm::scale(size);
	glm::mat4 matrix = projection * view * world * scale;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	activeShader->setUniformTexture("texS", t->getTextureID());
	glActiveTexture(GL_TEXTURE0);
	t->bindTexture();
	glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "texS"), 0);
	glUniform1f(glGetUniformLocation(activeShader->getShaderHandle(), "enableLighting"), 0.0f);
	glUniform1f(glGetUniformLocation(activeShader->getShaderHandle(), "selectedCard"), 0.0f);

	card->render();
}

void Game::drawRect(float x, float y, float z)
{
	glm::mat4 world;
	world = glm::translate(glm::vec3(x,y,z));
	glm::mat4 matrix = projection * view * world;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

void Game::drawRect(glm::vec3 direction, glm::vec3 position, glm::vec3 up, glm::vec3 size)
{
	glm::mat4 world, scale;
	world = glm::inverse((glm::lookAt(position, position+direction, up)));
	scale = glm::scale(size);
	glm::mat4 matrix = projection * view * world * scale;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

void Game::drawRect(float x, float y, float z, float rotation)
{
	glm::mat4 world;
	world = glm::translate(glm::vec3(x,y,z));
	world = glm::rotate(world, rotation, glm::vec3(0,1,0));
	glm::mat4 matrix = projection * view * world;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

int Game::render3D(long totalElapsedTime) 
{
	if (state == 0) {
		
	}
	else if (state == 1) {
		if (topDownCamera) {
			camera.setPosition(0,10,0);
			camera.setLookAtPoint(0,0,0);
			camera.setUpVector(0,0,1);
			camera.calculateOrthographicProjection(&projection, topDownX*topDownZoom, topDownY*topDownZoom);
		}
		else
			camera.calculateProjection(&projection);

		camera.calculateView(&view);		

		glEnable(GL_BLEND);
		
		for (list<Card*>::iterator it = cardList->begin(); it != cardList->end(); it++)
			(*it)->render(totalElapsedTime);

		if (renderArena) {
			for (list<GameObject*>::iterator it = arenaObjects->begin(); it != arenaObjects->end(); it++)
				(*it)->render(totalElapsedTime);
		}

		if (!topDownCamera) {
			cursorTexture->bindTexture();
			drawRect(-camera.getLookAtVector(), camera.getPos() + camera.getLookAtVector(), camera.getUpVector(), glm::vec3(0.02f, 0.02f, 1));
		}

		if (isKeyDown(IM_SPACE)) renderTooltipCard(totalElapsedTime);
	}

	return 0;
}

int Game::render2D(long time)
{
	camera.calculateOrthographicProjection(&projection, (float)screenWidth, (float)screenHeight); 
	camera.calculateMenuView(&view);

	defaultShader->activate();
	for (list<GUIItem*>::iterator it = GUIList->begin(); it != GUIList->end(); it++)
			(*it)->render(time);

	//Draw Text
	startOrtho();
		glDepthFunc(GL_ALWAYS);

			textShader->activate();
			textShader->setUniformf3("textColour", 0.9f, 0.9f, 0.9f);
			ostringstream s;
			s << "FPS:" << (int)(frameRate+0.5);
			drawText(10, screenHeight-30, s.str().c_str());

			#if DEBUG //Debug text
				const unsigned char * ver = glGetString(GL_VERSION);
				string v = "";
				int i = 0;
				while (ver[i] != ' ') { v += ver[i]; i++; }

				float version = atof(v.c_str());

				s.str(string());
				s << "OpenGL Version: " << version;
				drawText(210, screenHeight-30, s.str().c_str());
			
				s.str(string());
				s << "Pos:";
				drawText(10, screenHeight-60, s.str().c_str());
				s.str(string());
				s << camera.getPos().x;
				drawText(65, screenHeight-60, s.str().c_str());
				s.str(string());
				s << camera.getPos().y;
				drawText(65, screenHeight-85, s.str().c_str());
				s.str(string());
				s << camera.getPos().z;
				drawText(65, screenHeight-110, s.str().c_str());
			#endif
			
		glDepthFunc(GL_LEQUAL);
	endOrtho();
	defaultShader->activate();
	return 0;
}

//For Text Rendering
void Game::startOrtho() {
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
		glLoadIdentity ();
		glOrtho (0.0, (GLfloat) screenWidth, 0.0, (GLfloat) screenHeight, -100.0f, 100.0f);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
			glLoadIdentity ();
}

void Game::endOrtho() {
			glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

int Game::drawText(int x, int y, const char * s)
{
	if (s == NULL) return -1;

	char        text[256];              // Holds Our String
	va_list     ap;                 // Pointer To List Of Arguments

	va_start(ap, s);                  // Parses The String For Variables
	vsprintf(text, s, ap);                // And Converts Symbols To Actual Numbers
	va_end(ap);                     // Results Are Stored In Text

									//Determine the end of the string and convert each occurrence of '\n' to '\0'.
	char *end = text + strlen(text);
	for (char *next = text; *next != '\0'; next++) {
		if (*next == '\n') *next = '\0';
	}

	glColor4f(0.95f, 0.95f, 0.8f, 1.0f);

	long yOffset = y;
	for (char *line = text; line < end; line += strlen(line) + 1) {
		glWindowPos2i(x, y);//(yOffset-=32));
							//glRasterPos2i (x, yOffset); yOffset -= 32;
		glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits     ( NEW )
		glListBase(base - 32);                  // Sets The Base Character to 32    ( NEW )

		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text  ( NEW )
		glPopAttrib();
	}

	return 0;
}

#pragma endregion