#pragma once
#include "stdafx.h"

#include "GameState.h"
#include "AudioManager.h"
#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "InputManager.h"

namespace LD
{
	class PlayState;
	class GameManager
	{
	public:

		GameManager();
		~GameManager();

		// basic setup/update stuff
		void init();
		void deinit();
		void go();

		// state management
		void addState(GameState* state);
		void endCurrentState();
		void _forceQuit();

		PlayState* getState();

	//protected:

		bool mInitialized;
		std::vector<GameState*> mStates;
		AudioManager* mAudio;
		GraphicsManager* mGfx;
		InputManager* mInput;
		PhysicsManager* mPhysics;

		//chaiscript::ChaiScript mChai;
			
		bool mEndState;
		bool mQuit;

		int difficulty;

	};
}