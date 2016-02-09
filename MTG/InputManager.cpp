#include "InputManager.h"

InputManager::InputManager()
{
	mousePosition.x = mousePosition.y = 0;
	mouseMovement.x = mouseMovement.y = 0;
	mouseScrollY = 0;
}

void InputManager::init(long * time)
{
	for (int i = 0; i <= HIGHEST_KEY; i++)
	{
		keys[i].lastDown = 0;
		keys[i].lastUp = 1;
	}

	this->time = time;
}

InputManager::~InputManager()
{

}

long InputManager::convertSDLKToIM(long e)
{
	if (e > 122) 
	{
		switch (e)
		{
			case SDLK_LSHIFT:		return IM_LSHIFT;
			case SDLK_LCTRL:		return IM_LCTRL;
			case SDLK_LALT:			return IM_LALT;
			case SDLK_UP:			return IM_UP;
			case SDLK_DOWN:			return IM_DOWN;
			case SDLK_LEFT:			return IM_LEFT;
			case SDLK_RIGHT:		return IM_RIGHT;
			case SDLK_CAPSLOCK:		return IM_CAPSLOCK;
			case SDLK_PRINTSCREEN:	return IM_PRINTSCREEN;
			case SDLK_PAUSE:		return IM_PAUSE;
			case SDLK_SCROLLLOCK:	return IM_SCROLLLOCK;
			case SDLK_INSERT:		return IM_INSERT;
			case SDLK_HOME:			return IM_HOME;
			case SDLK_DELETE:		return IM_DELETE;
			case SDLK_END:			return IM_END;
			case SDLK_PAGEUP:		return IM_PAGEUP;
			case SDLK_PAGEDOWN:		return IM_PAGEDOWN;
			case SDLK_RSHIFT:		return IM_RSHIFT;
			case SDLK_RCTRL:		return IM_RCTRL;
			case SDLK_RALT:			return IM_RALT;	
			case SDLK_KP_ENTER:		return IM_NUM_ENTER;
			case SDLK_KP_PLUS:		return IM_NUM_PLUS;
			case SDLK_KP_MINUS:		return IM_NUM_MINUS;
			case SDLK_KP_MULTIPLY:	return IM_NUM_MULT;
			case SDLK_KP_DIVIDE:	return IM_NUM_DIV;
			case SDLK_KP_PERIOD:	return IM_NUM_PERIOD;
			case SDLK_KP_0:			return IM_NUM0;
			case SDLK_KP_1:			return IM_NUM1;
			case SDLK_KP_2:			return IM_NUM2;
			case SDLK_KP_3:			return IM_NUM3;
			case SDLK_KP_4:			return IM_NUM4;
			case SDLK_KP_5:			return IM_NUM5;
			case SDLK_KP_6:			return IM_NUM6;
			case SDLK_KP_7:			return IM_NUM7;
			case SDLK_KP_8:			return IM_NUM8;
			case SDLK_KP_9:			return IM_NUM9;
			case SDLK_F1:			return IM_F1;
			case SDLK_F2:			return IM_F2;
			case SDLK_F3:			return IM_F3;
			case SDLK_F4:			return IM_F4;
			case SDLK_F5:			return IM_F5;
			case SDLK_F6:			return IM_F6;
			case SDLK_F7:			return IM_F7;
			case SDLK_F8:			return IM_F8;
			case SDLK_F9:			return IM_F9;
			case SDLK_F10:			return IM_F10;
			case SDLK_F11:			return IM_F11;
			case SDLK_F12:			return IM_F12;
			default:				return -1;
		}
	}
	else if (e > -1) return e;
	return -1;
}

void InputManager::keyDown(long e)
{
	e = convertSDLKToIM(e);	
	if (isKeyDown(e)) return;
	keys[e].lastDown = *time;
}

void InputManager::keyUp(long e) { keys[convertSDLKToIM(e)].lastUp = *time; }

void InputManager::mouseKeyDown(long e) 
{	
	if (e < 1 || e > 20) 
		return;
	keys[e+122].lastDown = *time;
}

void InputManager::mouseKeyUp(long e)
{
	if (e < 1 || e > 20) 
		return;
	keys[e+122].lastUp = *time;
}

bool InputManager::isKeyDown(long e) { return keys[e].lastDown > keys[e].lastUp; }
bool InputManager::isKeyUp(long e) { return !isKeyDown(e); }
bool InputManager::isKeyPressed(long e) { return isKeyDown(e) && (keys[e].lastDown - *time) == 0; }
bool InputManager::isKeyReleased(long e) { return !isKeyDown(e) && (keys[e].lastUp - *time) == 0; }
long InputManager::getHoldDuration(long e) { return keys[e].lastUp - keys[e].lastDown; } 
long InputManager::getReleaseDuration(long e) { return keys[e].lastDown - keys[e].lastUp; } 
long InputManager::getLastDown(long e) { return keys[e].lastDown; }
long InputManager::getLastUp(long e) { return keys[e].lastUp; }