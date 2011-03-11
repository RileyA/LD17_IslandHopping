#pragma once
#include "stdafx.h"
#include "GameManager.h"
#include "GfxObj.h"
#include "CharacterController.h"
#include "Island.h"

namespace LD
{
	struct scoreText
	{
		scoreText()
		{
			mNr = 1;
			displaying = false;
			dur = 0.f;
		}
		scoreText(unsigned int nr)
		{
			mNr = nr;
			displaying = false;
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(nr))->hide();
			orX = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(nr))->getLeft();
			orY = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(nr))->getTop();
			dur = 0.f;
		}
		~scoreText()
		{

		}

		void setCaption(std::string text,unsigned int status,float _dur)
		{
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->show();
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setCaption(text);
			durSt = _dur;
			dur = _dur;
			displaying = true;

			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setLeft(orX+randFloat(-0.075f,0.075f));
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setTop(orY+randFloat(-0.075f,0.075f));

			Ogre::ColourValue val;
			if(status==0){val=Ogre::ColourValue(0.8f,0.8f,1.f,0.f);}
			if(status==1){val=Ogre::ColourValue(0.4f,1.f,0.5f,0.f);}
			if(status==2){val=Ogre::ColourValue(1.f,0.3f,0.3f,0.f);}
			if(status==3){val=Ogre::ColourValue(1.f,0.6f,0.32f,0.f);}

			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setColour(val);

		}

		void update(float delta)
		{
			dur-=delta;
			if(dur<1.f&&displaying)
			{
				Ogre::ColourValue val = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->getColour();
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setColour(Ogre::ColourValue(val.r,val.g,val.b,dur));
			}
			else if(dur>=durSt-0.25f)
			{
				Ogre::ColourValue val = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->getColour();
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setColour(Ogre::ColourValue(val.r,val.g,val.b,(durSt-dur)*4));
			}
			else
			{
				Ogre::ColourValue val = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->getColour();
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->setColour(Ogre::ColourValue(val.r,val.g,val.b,1));
			}

			if(dur<=0)
			{
				dur = 0;
				displaying = false;
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("Score"+Ogre::StringConverter::toString(mNr))->hide();
			}
		}

		bool done()
		{
			return displaying;
		}

		unsigned int mNr;
		float dur;
		float durSt;
		bool displaying;

		float orX;
		float orY;

	};
	class PlayState : public GameState
	{
	public:

		PlayState(GameManager* gmgr);

		~PlayState();

		virtual void init();

		virtual void deinit();

		virtual void update(float delta);

		Island* createIsland(std::string mesh,float px,float py,float pz,float speed,int type);

		void generateIslands();
		void PlayState::generateArrangement(unsigned int step);

		void setScore(std::string cap,unsigned int stat,float dur=2.f);

		PlayState()
			:GameState(NULL)
		{

		}

		std::vector<GfxObj*> mGfxObjects;
		GfxObj* testObj;
		GfxObj* testObj2;
		GfxObj* water1;
		GfxObj* water2;
		GfxObj* water3;
		GfxObj* water4;
		CharacterController* cc;
		std::vector<Island*> mIsles;
		std::vector<Island*> mInactiveIsles;

		unsigned int nrSteps;
		bool spaceDown;
		float jumpCharge;
		float STEPSCALE;

		unsigned int nrIslands;
		__int64 score;

		unsigned int startTime;

		int naut;

		scoreText scores[4];
		int combo;

		float multiplier;
		float dur;

		int max_dist;

		bool deathScreen;

		int minsLast;
		int secsLast;
		int millsLast;
		int max_combo;
		//float 
	};
}