#include "stdafx.h"
#include "OISManager.h"
#include "InputManager.h"

//* recycled input code... originally from the Ogre wiki I believe, then modified for an older project */

namespace LD
{
	OISManager::OISManager()
		:mInputManager(0),
		 mInputMgr(0),
		 mKeyboard(0),
		 mMouse(0),
		 mCursorX(0),
		 mCursorY(0)
	{

	}

	OISManager::~OISManager()
	{
		deinitialize();
	}


	void OISManager::initialize(size_t _handle,InputManager* inputMgr)
	{
		createInput(_handle);
		mInputMgr = inputMgr;
		mInitialized = true;
	}

	void OISManager::deinitialize()
	{
		if(mInitialized)
		{
			destroyInput();
			mInitialized = false;
		}
	}

	void OISManager::update()
	{
		captureInput();
	}

	void OISManager::createInput(size_t _handle)
	{
		std::ostringstream windowHndStr;
		windowHndStr << _handle;

		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = OIS::InputManager::createInputSystem(pl);

		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
		mKeyboard->setEventCallback(this);

		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
		mMouse->setEventCallback(this);
	}

	void OISManager::destroyInput()
	{
		if (mInputManager)
		{
			if (mMouse)
			{
				mInputManager->destroyInputObject( mMouse );
				mMouse = NULL;
			}
			if (mKeyboard)
			{
				mInputManager->destroyInputObject( mKeyboard );
				mKeyboard = NULL;
			}
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = NULL;
		}
	}

	bool OISManager::mouseMoved(const OIS::MouseEvent& _arg)
	{
		mCursorX += _arg.state.X.rel;
		mCursorY += _arg.state.Y.rel;

		checkPosition();

		if(mInputMgr)
		{
			mInputMgr->injectMouseMove(_arg.state.X.rel, _arg.state.Y.rel, _arg.state.Z.rel);
		}

		return true;
	}

	bool OISManager::mousePressed(const OIS::MouseEvent& _arg, OIS::MouseButtonID _id)
	{
		if(mInputMgr)
		{
			mInputMgr->injectMousePress(mCursorX, mCursorY, _id);
		}

		return true;
	}

	bool OISManager::mouseReleased(const OIS::MouseEvent& _arg, OIS::MouseButtonID _id)
	{
		if(mInputMgr)
		{
			mInputMgr->injectMouseRelease(mCursorX, mCursorY, _id);
		}

		return true;
	}

	bool OISManager::keyPressed(const OIS::KeyEvent& _arg)
	{
		if(mInputMgr)
		{
			mInputMgr->injectKeyPress(_arg.key, 'a');
		}

		return true;
	}

	bool OISManager::keyReleased(const OIS::KeyEvent& _arg)
	{
		if(mInputMgr)
		{
			mInputMgr->injectKeyRelease(_arg.key);
		}

		return true;
	}

	void OISManager::captureInput()
	{
		if (mMouse) mMouse->capture();
		mKeyboard->capture();
	}

	void OISManager::setInputViewSize(int _width, int _height)
	{
		if (mMouse)
		{
			const OIS::MouseState &ms = mMouse->getMouseState();
			ms.width = _width;
			ms.height = _height;

			checkPosition();
		}
	}

	void OISManager::setMousePosition(int _x, int _y)
	{
		//const OIS::MouseState &ms = mMouse->getMouseState();
		mCursorX = _x;
		mCursorY = _y;

		checkPosition();
	}

	void OISManager::checkPosition()
	{
		const OIS::MouseState &ms = mMouse->getMouseState();

		if (mCursorX < 0)
			mCursorX = 0;
		else if (mCursorX >= ms.width)
			mCursorX = ms.width - 1;

		if (mCursorY < 0)
			mCursorY = 0;
		else if (mCursorY >= ms.height)
			mCursorY = ms.height - 1;
	}

	void OISManager::updateCursorPosition()
	{
		const OIS::MouseState &ms = mMouse->getMouseState();

	}

	OIS::Mouse* OISManager::getMouse()
	{
		return mMouse;
	}

}
