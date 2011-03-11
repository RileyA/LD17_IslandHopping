#include "stdafx.h"
#include "MenuState.h"

#include "PlayState.h"

extern LD::MenuState Menu_State;
extern LD::PlayState Play_State;

namespace LD
{
	MenuState::MenuState(GameManager* gmgr)
		:GameState(gmgr)
	{
		selection = 0;
		txFlter = 3;
		difficulty = 0;
		upUp = 1;
		downUp = 1;
		gmgr->difficulty = difficulty;
	}

	MenuState::~MenuState()
	{
		
	}

	void MenuState::init()
	{
		Ogre::OverlayManager::getSingletonPtr()->getByName("MainMenu")->show();
		mGameManager->mGfx->mViewport->setBackgroundColour(Ogre::ColourValue(0.2,0.2,0.3));
		getElement(selection)->setColour(Ogre::ColourValue(0.9f, 0.6f, 0.2f));
		setHelp();

	}

	void MenuState::deinit()
	{
		Ogre::OverlayManager::getSingletonPtr()->getByName("MainMenu")->hide();
		mGameManager->mGfx->mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	}

	void MenuState::update(float delta)
	{
		if(mGameManager->mInput->isKeyDown("KC_RETURN"))
		{
			switch(selection)
			{
			case 0:
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuSelect.wav",false);
				Play_State = PlayState(mGameManager);
				mGameManager->addState(&Play_State);
				mGameManager->endCurrentState();
				break;
			case 1:
				// nothin'
				break;
			case 2:
				// nothin
				break;
			case 3:
				mGameManager->_forceQuit();
				break;
			}
		}
		if(mGameManager->mInput->isKeyDown("KC_DOWN")&&downUp)
		{	
			downUp = false;
			getElement(selection)->setColour(Ogre::ColourValue(0.7f, 0.7f, 0.9f));
			if(selection>=3)
			{
				selection = 0;
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuMove.wav",false);
			}
			else
			{
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuMove.wav",false);
				selection++;
			}
			getElement(selection)->setColour(Ogre::ColourValue(0.9f, 0.6f, 0.2f));
			setHelp();
		}
		else if(!downUp&&!mGameManager->mInput->isKeyDown("KC_DOWN"))
		{
			downUp = 1;
		}
		if(mGameManager->mInput->isKeyDown("KC_UP")&&upUp)
		{
			upUp = false;
			getElement(selection)->setColour(Ogre::ColourValue(0.7f, 0.7f, 0.9f));
			if(selection<=0)
			{
				selection = 3;
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuMove.wav",false);
			}
			else
			{
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuMove.wav",false);
				selection--;
			}
			getElement(selection)->setColour(Ogre::ColourValue(0.9f, 0.6f, 0.2f));
			setHelp();
		}
		else if(!upUp&&!mGameManager->mInput->isKeyDown("KC_UP"))
		{
			upUp = 1;
		}
		if(mGameManager->mInput->isKeyDown("KC_RIGHT")&&leftUp)
		{
			leftUp = false;
			rightUp = false;
			if(selection==1)
			{
				if(difficulty>=3)
				{
					//difficulty = 0;
				}
				else
				{
					mGameManager->mAudio->mEngine->play2D("../media/audio/menuSelect.wav",false);
					difficulty++;
				}
				if(difficulty==0){getElement(1)->setCaption("Difficulty: Easy");};
				if(difficulty==1){getElement(1)->setCaption("Difficulty: Medium");};
				if(difficulty==2){getElement(1)->setCaption("Difficulty: Hard");};
				if(difficulty==3){getElement(1)->setCaption("Difficulty: Insane");};
				mGameManager->difficulty = difficulty;
			}
			if(selection==2)
			{
				if(txFlter>=3)
				{
					//txFlter = 0;
				}
				else
				{
					mGameManager->mAudio->mEngine->play2D("../media/audio/menuSelect.wav",false);
					txFlter++;
				}
				if(txFlter==0){getElement(2)->setCaption("Texture Filtering: Low");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_BILINEAR);};// bilinear
				if(txFlter==1){getElement(2)->setCaption("Texture Filtering: Medium");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);};// trilinear
				if(txFlter==2){getElement(2)->setCaption("Texture Filtering: High");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);Ogre::MaterialManager::getSingletonPtr()->setDefaultAnisotropy(4);};// aniso x4
				if(txFlter==3){getElement(2)->setCaption("Texture Filtering: Max");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);Ogre::MaterialManager::getSingletonPtr()->setDefaultAnisotropy(16);};// aniso x16
			}
		}
		else if(!leftUp&&!mGameManager->mInput->isKeyDown("KC_RIGHT"))
		{
			leftUp = 1;
		}
		if(mGameManager->mInput->isKeyDown("KC_LEFT")&&rightUp)
		{
			rightUp = false;
			if(selection==1)
			{
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuSelect.wav",false);
				if(difficulty<=0)
				{
					//difficulty = 3;
				}
				else
				{
					difficulty--;
				}
				if(difficulty==0){getElement(1)->setCaption("Difficulty: Easy");};
				if(difficulty==1){getElement(1)->setCaption("Difficulty: Medium");};
				if(difficulty==2){getElement(1)->setCaption("Difficulty: Hard");};
				if(difficulty==3){getElement(1)->setCaption("Difficulty: Insane");};
				mGameManager->difficulty = difficulty;
			}
			if(selection==2)
			{
				mGameManager->mAudio->mEngine->play2D("../media/audio/menuSelect.wav",false);
				if(txFlter<=0)
				{
					//txFlter = 3;
				}
				else
				{
					txFlter--;
				}
				if(txFlter==0){getElement(2)->setCaption("Texture Filtering: Low");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_BILINEAR);};// bilinear
				if(txFlter==1){getElement(2)->setCaption("Texture Filtering: Medium");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);};// trilinear
				if(txFlter==2){getElement(2)->setCaption("Texture Filtering: High");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);Ogre::MaterialManager::getSingletonPtr()->setDefaultAnisotropy(4);};// aniso x4
				if(txFlter==3){getElement(2)->setCaption("Texture Filtering: Max");Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);Ogre::MaterialManager::getSingletonPtr()->setDefaultAnisotropy(16);};// aniso x16
			}
		}
		else if(!rightUp&&!mGameManager->mInput->isKeyDown("KC_LEFT"))
		{
			rightUp = 1;
		}
	}

	Ogre::OverlayElement* MenuState::getElement(unsigned int sel)
	{
		switch(sel)
		{
		case 0:
			return Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("OptionPlay");
			break;
		case 1:
			return Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("OptionDifficulty");
			break;
		case 2:
			return Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("OptionSensitivity");
			break;
		case 3:
			return Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("OptionQuit");
			break;
		case 4:
			return Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("HelpText");
			break;
		}
		return NULL;
	}

	void MenuState::setHelp()
	{
		switch(selection)
		{
		case 0:
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("HelpText")->setCaption("Play: Press ENTER to play!");
			break;
		case 1:
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("HelpText")->setCaption("Difficulty: Use L/R arrows to adjust.");
			break;
		case 2:
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("HelpText")->setCaption("Texture Filtering: Use L/R arrows to adjust.");
			break;
		case 3:
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("HelpText")->setCaption("Quit: Press ENTER to quit.");
			break;
		}
	}
}