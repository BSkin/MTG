#include "Game.h"

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR pCmdLine,
	int nCmdShow)
{
	Game game("MTG");
	game.init();
	game.start();
	game.cleanup();

	exit(0);

    return 0;
}