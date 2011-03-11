#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "stdafx.h"

//* recycled input code... originally from the Ogre wiki I believe, then modified for an older project */

namespace LD
{
	// forward declaration
	class OISManager;

	class InputManager
	{
	public:

		InputManager();

		~InputManager();

		void update();

		void initialize(size_t _handle);

		void deinitialize();


		bool isKeyDown(const unsigned int& code);
		bool isKeyDown(const std::string& code);

		bool isMouseButtonDown(unsigned int code);
		bool isMouseButtonDown(std::string code);

		void assignKeyConversions();

		void injectMouseMove(int absx, int absy, int absz);
		void injectMousePress(int absx, int absy, unsigned int id);
		void injectMouseRelease(int absx, int absy, unsigned int id);
		void injectKeyPress(unsigned int key, unsigned int text);
		void injectKeyRelease(unsigned int key);

		void setWindowSize(unsigned int w,unsigned int h);

		int getMouseX();
		int getMouseY();
		int getMouseZ();

		OISManager* getOISManager(){return mOISManager;};

	protected:


	private:

		std::map<unsigned int,std::string> mKeyConversions;
		std::vector<std::string> mMouseConversions;
		std::map<unsigned int,bool> mKeyStates;
		std::vector<bool> mMouseStates;
		std::map<std::string, unsigned int> mKeyNames;
		std::map<std::string, unsigned int> mMouseNames;

		OISManager* mOISManager;

		bool mInitialized;


		int mMouseX;
		int mMouseY;
		int mMouseZ;

	};
}

#endif