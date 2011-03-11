#include "stdafx.h"
#include "InputManager.h"
#include "OISManager.h"
#include "OIS.h"

//* recycled input code... originally from the Ogre wiki I believe, then modified for an older project */

namespace LD
{
	InputManager::InputManager()
		:mKeyStates(),
		mKeyConversions(),
		mKeyNames(),
		mOISManager(0)
	{
		mOISManager = new OISManager;
	}

	InputManager::~InputManager()
	{
		deinitialize();
		delete mOISManager;
		mOISManager = 0;
	}

	void InputManager::update()
	{
		//mMouseX = 0;
		//mMouseY = 0;
		mMouseZ = 0;
		mOISManager->update();
	}

	void InputManager::initialize(size_t _handle)
	{
		assignKeyConversions();
		std::map<unsigned int,std::string>::iterator iter = mKeyConversions.begin();
		for(;iter!=mKeyConversions.end();++iter)
		{
			mKeyStates[iter->first] = false;
			mKeyNames[iter->second] = iter->first;
		}

		for(unsigned int i=0;i<mMouseConversions.size();++i)
		{
			mMouseStates.push_back(false);
			mMouseNames[mMouseConversions[i]] = i;
		}

		mOISManager->initialize(_handle,this);

		mInitialized = true;
	}

	void InputManager::deinitialize()
	{
		if(mInitialized)
		{
			mOISManager->deinitialize();
			mInitialized = false;
		}
	}

	void InputManager::injectMouseMove(int absx, int absy, int absz)
	{
		mMouseX = absx;
		mMouseY = absy;
		mMouseZ = absz;
	}

	void InputManager::injectMousePress(int absx, int absy, unsigned int id)
	{
		mMouseStates[id] = true;
	}

	void InputManager::injectMouseRelease(int absx, int absy, unsigned int id)
	{
		mMouseStates[id] = false;
	}

	void InputManager::injectKeyPress(unsigned int key, unsigned int text)
	{
		mKeyStates[key] = true;
	}

	void InputManager::injectKeyRelease(unsigned int key)
	{
		mKeyStates[key] = false;
	}

	bool InputManager::isKeyDown(const unsigned int& code)
	{
		return mKeyStates[code];
	}

	bool InputManager::isKeyDown(const std::string& code)
	{
		return mKeyStates[mKeyNames[code]];
	}

	bool InputManager::isMouseButtonDown(unsigned int code)
	{
		return mMouseStates[code];
	}
	bool InputManager::isMouseButtonDown(std::string code)
	{
		return mMouseStates[mMouseNames[code]];
	}

	void InputManager::setWindowSize(unsigned int w,unsigned int h)
	{
		mOISManager->setInputViewSize(w,h);
	}

	 int InputManager::getMouseX()
	 {
		return mOISManager->getMouse()->getMouseState().X.rel;
		//int ret = mMouseX;
		//mMouseX = 0;
		//return ret;
	}

	 int InputManager::getMouseY()
	{
		return mOISManager->getMouse()->getMouseState().Y.rel;
		//int ret = mMouseY;
		//mMouseY = 0;
		//return ret;
	}

	 int InputManager::getMouseZ()
	{
		return mMouseZ;
	}

	void InputManager::assignKeyConversions()
	{
		mMouseConversions.push_back(std::string("MB_Left"));
		mMouseConversions.push_back(std::string("MB_Right"));
		mMouseConversions.push_back(std::string("MB_Middle"));
		mMouseConversions.push_back(std::string("MB_Button3"));
		mMouseConversions.push_back(std::string("MB_Button4"));
		mMouseConversions.push_back(std::string("MB_Button5"));
		mMouseConversions.push_back(std::string("MB_Button6"));
		mMouseConversions.push_back(std::string("MB_Button7"));

		mKeyConversions[OIS::KC_UNASSIGNED] = std::string("KC_UNASSIGNED");
		mKeyConversions[OIS::KC_ESCAPE] = std::string("KC_ESCAPE");
		mKeyConversions[OIS::KC_1] = std::string("KC_1");
		mKeyConversions[OIS::KC_2] = std::string("KC_2");
		mKeyConversions[OIS::KC_3] = std::string("KC_3");
		mKeyConversions[OIS::KC_4] = std::string("KC_4");
		mKeyConversions[OIS::KC_5]  = std::string("KC_5");
		mKeyConversions[OIS::KC_6] = std::string("KC_6");
		mKeyConversions[OIS::KC_7] = std::string("KC_7");
		mKeyConversions[OIS::KC_8] = std::string("KC_8");
		mKeyConversions[OIS::KC_9] = std::string("KC_9");
		mKeyConversions[OIS::KC_0] = std::string("KC_0");
		mKeyConversions[OIS::KC_MINUS] = std::string("KC_MINUS");
		mKeyConversions[OIS::KC_EQUALS] = std::string("KC_EQUALS");
		mKeyConversions[OIS::KC_BACK] = std::string("KC_BACK");
		mKeyConversions[OIS::KC_TAB] = std::string("KC_TAB");
		mKeyConversions[OIS::KC_Q] = std::string("KC_Q");
		mKeyConversions[OIS::KC_W] = std::string("KC_W");
		mKeyConversions[OIS::KC_E] = std::string("KC_E");
		mKeyConversions[OIS::KC_R] = std::string("KC_R");
		mKeyConversions[OIS::KC_T] = std::string("KC_T");
		mKeyConversions[OIS::KC_Y] = std::string("KC_Y");
		mKeyConversions[OIS::KC_U] = std::string("KC_U");
		mKeyConversions[OIS::KC_I] = std::string("KC_I");
		mKeyConversions[OIS::KC_O] = std::string("KC_O");
		mKeyConversions[OIS::KC_P] = std::string("KC_P");
		mKeyConversions[OIS::KC_LBRACKET] = std::string("KC_LBRACKET");
		mKeyConversions[OIS::KC_RBRACKET] = std::string("KC_RBRACKET");
		mKeyConversions[OIS::KC_RETURN] = std::string("KC_RETURN");
		mKeyConversions[OIS::KC_LCONTROL] = std::string("KC_LCONTROL");
		mKeyConversions[OIS::KC_A] = std::string("KC_A");
		mKeyConversions[OIS::KC_S] = std::string("KC_S");
		mKeyConversions[OIS::KC_D] = std::string("KC_D");
		mKeyConversions[OIS::KC_F] = std::string("KC_F");
		mKeyConversions[OIS::KC_G] = std::string("KC_G");
		mKeyConversions[OIS::KC_H] = std::string("KC_H");
		mKeyConversions[OIS::KC_J] = std::string("KC_J");
		mKeyConversions[OIS::KC_K] = std::string("KC_K");
		mKeyConversions[OIS::KC_L] = std::string("KC_L");
		mKeyConversions[OIS::KC_SEMICOLON] = std::string("KC_SEMICOLON");
		mKeyConversions[OIS::KC_APOSTROPHE] = std::string("KC_APOSTROPHE");
		mKeyConversions[OIS::KC_GRAVE] = std::string("KC_GRAVE");
		mKeyConversions[OIS::KC_LSHIFT] = std::string("KC_LSHIFT");
		mKeyConversions[OIS::KC_BACKSLASH] = std::string("KC_BACKSLASH");
		mKeyConversions[OIS::KC_Z] = std::string("KC_Z");
		mKeyConversions[OIS::KC_X] = std::string("KC_X");
		mKeyConversions[OIS::KC_C] = std::string("KC_C");
		mKeyConversions[OIS::KC_V] = std::string("KC_V");
		mKeyConversions[OIS::KC_B] = std::string("KC_B");
		mKeyConversions[OIS::KC_N] = std::string("KC_N");
		mKeyConversions[OIS::KC_M] = std::string("KC_M");
		mKeyConversions[OIS::KC_COMMA] = std::string("KC_COMMA");
		mKeyConversions[OIS::KC_PERIOD] = std::string("KC_PERIOD");
		mKeyConversions[OIS::KC_SLASH] = std::string("KC_SLASH");
		mKeyConversions[OIS::KC_RSHIFT] = std::string("KC_RSHIFT");
		mKeyConversions[OIS::KC_MULTIPLY] = std::string("KC_MULTIPLY");
		mKeyConversions[OIS::KC_LMENU] = std::string("KC_LMENU");
		mKeyConversions[OIS::KC_SPACE] = std::string("KC_SPACE");
		mKeyConversions[OIS::KC_CAPITAL] = std::string("KC_CAPITAL");
		mKeyConversions[OIS::KC_F1] = std::string("KC_F1");
		mKeyConversions[OIS::KC_F2] = std::string("KC_F2");
		mKeyConversions[OIS::KC_F3] = std::string("KC_F3");
		mKeyConversions[OIS::KC_F4] = std::string("KC_F4");
		mKeyConversions[OIS::KC_F5] = std::string("KC_F5");
		mKeyConversions[OIS::KC_F6] = std::string("KC_F6");
		mKeyConversions[OIS::KC_F7] = std::string("KC_F7");
		mKeyConversions[OIS::KC_F8] = std::string("KC_F8");
		mKeyConversions[OIS::KC_F9] = std::string("KC_F9");
		mKeyConversions[OIS::KC_F10] = std::string("KC_F10");
		mKeyConversions[OIS::KC_NUMLOCK] = std::string("KC_NUMLOCK");
		mKeyConversions[OIS::KC_SCROLL] = std::string("KC_SCROLL");
		mKeyConversions[OIS::KC_NUMPAD7] = std::string("KC_NUMPAD7");
		mKeyConversions[OIS::KC_NUMPAD8] = std::string("KC_NUMPAD8");
		mKeyConversions[OIS::KC_NUMPAD9] = std::string("KC_NUMPAD9");
		mKeyConversions[OIS::KC_SUBTRACT] = std::string("KC_SUBTRACT");
		mKeyConversions[OIS::KC_NUMPAD4] = std::string("KC_NUMPAD4");
		mKeyConversions[OIS::KC_NUMPAD5] = std::string("KC_NUMPAD5");
		mKeyConversions[OIS::KC_NUMPAD6] = std::string("KC_NUMPAD6");
		mKeyConversions[OIS::KC_ADD] = std::string("KC_ADD");
		mKeyConversions[OIS::KC_NUMPAD1] = std::string("KC_NUMPAD1");
		mKeyConversions[OIS::KC_NUMPAD2] = std::string("KC_NUMPAD2");
		mKeyConversions[OIS::KC_NUMPAD3] = std::string("KC_NUMPAD3");
		mKeyConversions[OIS::KC_NUMPAD0] = std::string("KC_NUMPAD0");
		mKeyConversions[OIS::KC_DECIMAL] = std::string("KC_DECIMAL");
		mKeyConversions[OIS::KC_OEM_102] = std::string("KC_OEM_102");
		mKeyConversions[OIS::KC_F11] = std::string("KC_F11");
		mKeyConversions[OIS::KC_F12] = std::string("KC_F12");
		mKeyConversions[OIS::KC_F13] = std::string("KC_F13");
		mKeyConversions[OIS::KC_F14] = std::string("KC_F14");
		mKeyConversions[OIS::KC_F15] = std::string("KC_F15");
		mKeyConversions[OIS::KC_KANA] = std::string("KC_KANA");
		mKeyConversions[OIS::KC_ABNT_C1] = std::string("KC_ABNT_C1");
		mKeyConversions[OIS::KC_CONVERT] = std::string("KC_CONVERT");
		mKeyConversions[OIS::KC_NOCONVERT] = std::string("KC_NOCONVERT");
		mKeyConversions[OIS::KC_YEN] = std::string("KC_YEN");
		mKeyConversions[OIS::KC_ABNT_C2] = std::string("KC_ABNT_C2");
		mKeyConversions[OIS::KC_NUMPADEQUALS] = std::string("KC_NUMPADEQUALS");
		mKeyConversions[OIS::KC_PREVTRACK] = std::string("KC_PREVTRACK");
		mKeyConversions[OIS::KC_AT] = std::string("KC_AT");
		mKeyConversions[OIS::KC_COLON] = std::string("KC_COLON");
		mKeyConversions[OIS::KC_UNDERLINE] = std::string("KC_UNDERLINE");
		mKeyConversions[OIS::KC_KANJI] = std::string("KC_KANJI");
		mKeyConversions[OIS::KC_STOP] = std::string("KC_STOP");
		mKeyConversions[OIS::KC_AX] = std::string("KC_AX");
		mKeyConversions[OIS::KC_UNLABELED ] = std::string("KC_UNLABELED");
		mKeyConversions[OIS::KC_NEXTTRACK] = std::string("KC_NEXTTRACK");
		mKeyConversions[OIS::KC_NUMPADENTER] = std::string("KC_NUMPADENTER");
		mKeyConversions[OIS::KC_RCONTROL] = std::string("KC_RCONTROL");
		mKeyConversions[OIS::KC_MUTE] = std::string("KC_MUTE");
		mKeyConversions[OIS::KC_CALCULATOR] = std::string("KC_CALCULATOR");
		mKeyConversions[OIS::KC_PLAYPAUSE] = std::string("KC_PLAYPAUSE");
		mKeyConversions[OIS::KC_MEDIASTOP] = std::string("KC_MEDIASTOP");
		mKeyConversions[OIS::KC_VOLUMEDOWN] = std::string("KC_VOLUMEDOWN");
		mKeyConversions[OIS::KC_VOLUMEUP] = std::string("KC_VOLUMEUP");
		mKeyConversions[OIS::KC_WEBHOME] = std::string("KC_WEBHOME");
		mKeyConversions[OIS::KC_NUMPADCOMMA] = std::string("KC_NUMPADCOMMA");
		mKeyConversions[OIS::KC_DIVIDE] = std::string("KC_DIVIDE");
		mKeyConversions[OIS::KC_SYSRQ] = std::string("KC_SYSRQ");
		mKeyConversions[OIS::KC_RMENU] = std::string("KC_RMENU");
		mKeyConversions[OIS::KC_PAUSE] = std::string("KC_PAUSE");
		mKeyConversions[OIS::KC_HOME] = std::string("KC_HOME");
		mKeyConversions[OIS::KC_UP] = std::string("KC_UP");
		mKeyConversions[OIS::KC_PGUP] = std::string("KC_PGUP");
		mKeyConversions[OIS::KC_LEFT] = std::string("KC_LEFT");
		mKeyConversions[OIS::KC_RIGHT] = std::string("KC_RIGHT");
		mKeyConversions[OIS::KC_END] = std::string("KC_END");
		mKeyConversions[OIS::KC_DOWN] = std::string("KC_DOWN");
		mKeyConversions[OIS::KC_PGDOWN] = std::string("KC_PGDOWN");
		mKeyConversions[OIS::KC_INSERT] = std::string("KC_INSERT");
		mKeyConversions[OIS::KC_DELETE] = std::string("KC_DELETE");
		mKeyConversions[OIS::KC_LWIN] = std::string("KC_LWIN");
		mKeyConversions[OIS::KC_RWIN] = std::string("KC_RWIN");
		mKeyConversions[OIS::KC_APPS] = std::string("KC_APPS");
		mKeyConversions[OIS::KC_POWER] = std::string("KC_POWER");
		mKeyConversions[OIS::KC_SLEEP] = std::string("KC_SLEEP");
		mKeyConversions[OIS::KC_WAKE] = std::string("KC_WAKE");
		mKeyConversions[OIS::KC_WEBSEARCH] = std::string("KC_WEBSEARCH");
		mKeyConversions[OIS::KC_WEBFAVORITES] = std::string("KC_WEBFAVORITES");
		mKeyConversions[OIS::KC_WEBREFRESH] = std::string("KC_WEBREFRESH");
		mKeyConversions[OIS::KC_WEBSTOP] = std::string("KC_WEBSTOP");
		mKeyConversions[OIS::KC_WEBFORWARD] = std::string("KC_WEBFORWARD");
		mKeyConversions[OIS::KC_WEBBACK] = std::string("KC_WEBBACK");
		mKeyConversions[OIS::KC_MYCOMPUTER] = std::string("KC_MYCOMPUTER");
		mKeyConversions[OIS::KC_MAIL] = std::string("KC_MAIL");
		mKeyConversions[OIS::KC_MEDIASELECT] = std::string("KC_MEDIASELECT");
	}
}