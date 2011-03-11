#pragma once
#include "stdafx.h"

namespace LD
{
	class GameManager;

	class GameState
	{
	public:

		GameState(GameManager* gmgr)
			:mInitialized(0),mEndState(0),mGameManager(gmgr){}

		virtual void init()=0;
			
		virtual void deinit()=0;

		virtual void update(float delta)=0;

	//[not]protected

		bool mInitialized;
		bool mEndState;
		GameManager* mGameManager;

	};
}