#ifndef OISMANAGER_H
#define OISMANAGER_H
#include "stdafx.h"

//* recycled input code... originally from the Ogre wiki I believe, then modified for an older project */

#include "InputManager.h"

namespace LD
{
	// forward declare this here
	class InputManager;

	class OISManager
		: public OIS::MouseListener,
		  public OIS::KeyListener
	{
	public:

		OISManager();
		~OISManager();

		void initialize(size_t _handle,InputManager* inputMgr);

		void deinitialize();

		void update();

		void createInput(size_t _handle);
		void destroyInput();
		void captureInput();
		void setInputViewSize(int _width, int _height);

		//void injectMouseMove(int _absx, int _absy, int _absz) { }
		//void injectMousePress(int _absx, int _absy, MyGUI::MouseButton _id) { }
		//void injectMouseRelease(int _absx, int _absy, MyGUI::MouseButton _id) { }
		//void injectKeyPress(MyGUI::KeyCode _key, MyGUI::Char _text) { }
		//void injectKeyRelease(MyGUI::KeyCode _key) { }

		void setMousePosition(int _x, int _y);
		void updateCursorPosition();

		OIS::Mouse* getMouse();

	protected:

		bool mouseMoved(const OIS::MouseEvent& _arg);
		bool mousePressed(const OIS::MouseEvent& _arg, OIS::MouseButtonID _id);
		bool mouseReleased(const OIS::MouseEvent& _arg, OIS::MouseButtonID _id);
		bool keyPressed(const OIS::KeyEvent& _arg);
		bool keyReleased(const OIS::KeyEvent& _arg);

		void checkPosition();

	private:

		bool mInitialized;

		OIS::InputManager* mInputManager;
		OIS::Keyboard* mKeyboard;
		OIS::Mouse* mMouse;

		int mCursorX;
		int mCursorY;

		InputManager* mInputMgr;

	};
}
#endif