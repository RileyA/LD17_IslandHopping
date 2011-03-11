#pragma once

#include "stdafx.h"
#include "InputManager.h"
#include "OISManager.h"

namespace LD
{
	class GraphicsManager
	{
	public:

		GraphicsManager();
		~GraphicsManager();

		void update();

		void init(InputManager* input);

		void deinit();

	//[un]protected: I'm too lazy to make getters and setters for all this...

		void setCameraRot(bool enabled);

		bool mInitialized;
		Ogre::Root* mRoot;
		Ogre::Viewport* mViewport;
		Ogre::RenderWindow* mWindow;
		Ogre::Camera* mMainCam;
		Ogre::SceneManager* mSceneManager;

		// camera control stuffs...
		float mPitchLimitUp;
		float mPitchLimitDown;
		float mCurrentPitch;
		Ogre::SceneNode* mCameraRollNode;
		Ogre::SceneNode* mCameraYawNode;
		Ogre::SceneNode* mCameraPitchNode;
		Ogre::SceneNode* mCameraNode;
		Ogre::SceneNode* mCameraPosNode;
		bool camLook;

		InputManager* mInput;

		std::vector<float> mMouseSmoothX;
		std::vector<float> mMouseSmoothY;
		std::vector<float> mMouseSmoothTime;

		float mLookSensitivity;

		bool canLook;

	};
}