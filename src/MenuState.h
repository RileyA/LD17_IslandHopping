#pragma once
#include "stdafx.h"
#include "GameManager.h"

namespace LD
{
	class MenuState : public GameState
	{
	public:

		MenuState(GameManager* gmgr);
		~MenuState();

		void init();

		void deinit();

		void update(float delta);

		Ogre::OverlayElement* getElement(unsigned int sel);

		void setHelp();

		MenuState()
			:GameState(NULL)
		{

		}

		unsigned int selection;
		unsigned int txFlter;
		unsigned int difficulty;
		bool upUp;
		bool downUp;
		bool leftUp;
		bool rightUp;

	
	};
}