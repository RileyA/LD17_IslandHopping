#include "stdafx.h"
#include "PlayState.h"
#include "GameManager.h"
#include "MenuState.h"

extern LD::MenuState Menu_State;
extern LD::PlayState Play_State;

namespace LD
{
	PlayState::PlayState(GameManager* gmgr)
		:GameState(gmgr)
	{nrSteps = 0;spaceDown=false;jumpCharge=0.f;}

	PlayState::~PlayState()
	{
		deinit();
	}

	void PlayState::init()
	{
		mGameManager->mGfx->canLook = true;
		deathScreen = 0;
		max_dist = 0;
		multiplier = (mGameManager->difficulty+1);
		dur = 0.f;
		scores[0] = scoreText(1);
		scores[1] = scoreText(2);
		scores[2] = scoreText(3);
		scores[3] = scoreText(4);

		naut = 0;
		mInitialized = true;
		mGameManager->mPhysics->init();
		mGameManager->mGfx->setCameraRot(true);
		STEPSCALE = 1.f;
		testObj = new GfxObj("Sea.mesh");
		water1 = new GfxObj("Sea.mesh");
		water1->mNode->setPosition(Ogre::Vector3(0,5.9f,0));
		water1->mEntity->setMaterialName("water");
		mGfxObjects.push_back(testObj);
		mGfxObjects.push_back(water1);

				water2 = new GfxObj("Sea.mesh");
		water2->mNode->setPosition(Ogre::Vector3(0,5.9f,-200));
		water2->mEntity->setMaterialName("water");
		mGfxObjects.push_back(water2);

				water3 = new GfxObj("Sea.mesh");
		water3->mNode->setPosition(Ogre::Vector3(0,5.9f,-400));
		water3->mEntity->setMaterialName("water");
		mGfxObjects.push_back(water3);

				water4 = new GfxObj("Sea.mesh");
		water4->mNode->setPosition(Ogre::Vector3(0,5.9f,-600));
		water4->mEntity->setMaterialName("water");
		mGfxObjects.push_back(water4);

		//mGameManager->mPhysics->createStaticTrimesh("Island_Small.mesh",Ogre::Vector3(0,0,0),0.0f,0.f);
		mGameManager->mPhysics->createStaticTrimesh("Sea.mesh",Ogre::Vector3(0,0,0),0.0f,0.f);

		cc = mGameManager->mPhysics->createCCTCap(Ogre::Vector3(1,1,1),Ogre::Vector3(0,-0.479f,0),20.f,0.f,0.f);

		mGameManager->mGfx->mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0.f));

		Ogre::OverlayManager::getSingletonPtr()->getByName("HUD")->show();

		createIsland("Island_Runway.mesh",0,0,0,2,0);

		nrSteps = 5;
		nrIslands = 0;
		score = 0;
		startTime = clock();
		naut = 0;
		//scores[0].setCaption("+500",2,2.f);
		combo = 0;
		minsLast = 0;
		secsLast = 0;
		millsLast = 0;
		max_combo = 0;
	}

	void PlayState::deinit()
	{
		if(mInitialized)
		{
			Ogre::OverlayManager::getSingletonPtr()->getByName("POSTMORTEM")->hide();
			Ogre::OverlayManager::getSingletonPtr()->getByName("HUD")->hide();

		Ogre::OverlayManager::getSingletonPtr()->getByName("MainMenu")->hide();

			//delete testObj;
			for(unsigned int i = 0;i<mGfxObjects.size();++i)
			{
				delete mGfxObjects[i];
				mGfxObjects[i] = 0;
			}
			mGfxObjects.clear();
			for(unsigned int i=0;i<mIsles.size();++i)
			{
				delete mIsles[i];
				mIsles[i] = 0;
			}
			mIsles.clear();

			for(unsigned int i=0;i<mInactiveIsles.size();++i)
			{
				delete mInactiveIsles[i];
				mInactiveIsles[i] = 0;
			}
			mInactiveIsles.clear();

			mGameManager->mPhysics->deinit();
			mGameManager->mGfx->setCameraRot(false);
			std::cout<<"State Deinitialized!\n";
			mInitialized = 0;
		}
	}

	void PlayState::update(float delta)
	{

		scores[0].update(delta);
		scores[1].update(delta);
		scores[2].update(delta);
		scores[3].update(delta);

		if(!deathScreen)
		{
			if(combo>max_combo)
			{
				max_combo = combo;
			}
			for(unsigned int i=0;i<mIsles.size();++i)
			{
				if(abs(mIsles[i]->object->mNode->getPosition().z-cc->getPosition().z)>200)
				{
					mIsles[i]->setVisible(false);
					mInactiveIsles.push_back(mIsles[i]);
					mIsles.erase(mIsles.begin()+i);
					--i;
				}
				else
				{
					mIsles[i]->update(delta);
				}
			}

			generateIslands();

			mGameManager->mGfx->mCameraPosNode->setPosition(cc->getPosition()+Ogre::Vector3(0,0.2f,0));

			Ogre::Vector3 step = cc->getPosition();
			step/=20;
			int tempo = static_cast<int>(step.x); step.x = static_cast<float>(tempo);
			 tempo = static_cast<int>(step.z); step.z = static_cast<float>(tempo);
			 step.y = 0.2f;
			testObj->mNode->setPosition(step*20);

			if(cc->getPosition().y<-4)
			{
				mGameManager->mAudio->mEngine->play2D("../media/audio/fail_01.wav",false);
				deathScreen = true;
				cc->setPosition(cc->getPosition());
				mGameManager->mGfx->canLook = false;
				Ogre::OverlayManager::getSingletonPtr()->getByName("HUD")->hide();
				Ogre::OverlayManager::getSingletonPtr()->getByName("POSTMORTEM")->show();
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("PDistanceText")->setCaption("Distance: "+Ogre::StringConverter::toString(max_dist)+ " meters");
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("PScoreText")->setCaption("Score: "+Ogre::StringConverter::toString(static_cast<int>(score)));
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("PTimeText")->setCaption("Time: "+Ogre::StringConverter::toString(minsLast)+":"+Ogre::StringConverter::toString(secsLast)+":"+Ogre::StringConverter::toString(millsLast));
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("PIslesText")->setCaption("Highest Combo: "+Ogre::StringConverter::toString(max_combo));
				Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("PMultiText")->setCaption("Islands: "+Ogre::StringConverter::toString(nrIslands));
			}
		}

		if(deathScreen&&mGameManager->mInput->isKeyDown("KC_RETURN"))
		{
			Ogre::OverlayManager::getSingletonPtr()->getByName("HUD")->show();
			Ogre::OverlayManager::getSingletonPtr()->getByName("POSTMORTEM")->hide();
			mGameManager->mGfx->canLook = true;
			cc->setPosition(Ogre::Vector3(0,0,0));
			cc->mMove = Ogre::Vector3(0,0,0);
			mGameManager->mGfx->setCameraRot(false);
			mGameManager->mGfx->setCameraRot(true);

			for(unsigned int a=0;a<mIsles.size();++a)
			{
				mInactiveIsles.push_back(mIsles[a]);
				mIsles[a]->setVisible(false);
			}
			mIsles.clear();

			nrSteps = 5;
			cc->speed = 6.f;
			STEPSCALE = 1.f;
			nrIslands = 0;
			score = 0;
			startTime = clock();
			naut = 0;
			water1->mNode->setPosition(Ogre::Vector3(0,5.9,0));
			water2->mNode->setPosition(Ogre::Vector3(0,5.9,-200));
			water3->mNode->setPosition(Ogre::Vector3(0,5.9,-400));
			water4->mNode->setPosition(Ogre::Vector3(0,5.9,-600));
			createIsland("Island_Runway.mesh",0,0,0,2,0);
			combo = 0;
			multiplier = (mGameManager->difficulty+1);
			dur = 0.f;
			max_dist = 0;
			deathScreen = 0;
			max_combo = 0;
			minsLast = 0;
			secsLast = 0;
			millsLast = 0;
		}

		if(mGameManager->mInput->isKeyDown("KC_ESCAPE"))
		{
			mEndState = true;
			Menu_State = MenuState(mGameManager);
			mGameManager->addState(&Menu_State);
		}

		if(!deathScreen)
		{
			if(cc->getPosition().z<-200)
			{
				unsigned int nautical = -((cc->getPosition().z+200)/200.f);
				if(nautical>naut)
				{
					water1->mNode->setPosition((nautical)*Ogre::Vector3(0,0,-200)+Ogre::Vector3(0,5.9f,0));
					water2->mNode->setPosition((nautical+1)*Ogre::Vector3(0,0,-200)+Ogre::Vector3(0,5.9f,0));
					water3->mNode->setPosition((nautical+2)*Ogre::Vector3(0,0,-200)+Ogre::Vector3(0,5.9f,0));
					water4->mNode->setPosition((nautical+3)*Ogre::Vector3(0,0,-200)+Ogre::Vector3(0,5.9f,0));
					naut = nautical;
				}
			}

			//testObj2->mNode->setPosition(cc->getPosition()+Ogre::Vector3(0,6.f,0));

			/*cc->speed += mGameManager->mInput->getMouseZ()/(120.f*4);
			if(cc->speed>8.f)
			{
				cc->speed = 8.f;
			}
			if(cc->speed<4.f)
			{
				cc->speed = 4.f;
			}*/


			Ogre::Vector3 mv = Ogre::Vector3(0,0,0);
			//if(!cc->midAir)
			//{
				//if(mGameManager->mInput->isKeyDown("KC_W"))
				//{
				//	if(!cc->midAir)
				//	{
				//		cc->speed+=1.f/30.f;
				//		if(cc->speed>7.75f)
				//		{
				//			cc->speed = 7.75;
				//		}
				//	}
				//	//else
				//	//{
				//	//	cc->fallSpeed-=0.15f;
				//	//}
				//	//Ogre::Vector3 tmp = mGameManager->mGfx->mMainCam->getDerivedDirection();
				//	//tmp.y = 0;
				//	//tmp.normalise();
				//	//mv+=tmp;
				//}
				//if(mGameManager->mInput->isKeyDown("KC_S"))
				//{
				//	if(cc->midAir)
				//	{
				//		
				//		//{
				//			cc->fallSpeed+=0.075f;
				//		//}
				//	}
				//	cc->speed-=0.015f;
				//	if(cc->speed<4.f)
				//	{
				//		cc->speed = 4.f;
				//	}

				//	//cc->speed+=1.f/25.f;
				//	//Ogre::Vector3 tmp = -mGameManager->mGfx->mMainCam->getDerivedDirection();
				//	//tmp.y = 0;
				//	//tmp.normalise();
				//	//mv+=tmp;
				//}
				//if(mGameManager->mInput->isKeyDown("KC_A"))
				//{
				//	Ogre::Vector3 tmp = -mGameManager->mGfx->mMainCam->getDerivedRight();
				//	tmp.y = 0;
				//	tmp.normalise();
				//	mv+=tmp;
				//}
				//if(mGameManager->mInput->isKeyDown("KC_D"))
				//{
				//	Ogre::Vector3 tmp = mGameManager->mGfx->mMainCam->getDerivedRight();
				//	tmp.y = 0;
				//	tmp.normalise();
				//	mv+=tmp;
				//}


			

			//if(mGameManager->mInput->isKeyDown("KC_SPACE"))
			//{
			//	if(!spaceDown)
			//	{
			//	jumpCharge=2.2f;
			//	spaceDown = true;
			//	}
			//	else
			//	{
			//	jumpCharge+=1.f/10.f;
			//	if(jumpCharge>5.f)
			//	{
			//		jumpCharge = 5.f;
			//	}
			//	}
			//	//if(!cc->midAir)
			//	//{

			//	//}
			//	//else
			//	//{
			//	//	cc->fallSpeed+=1.f/30.f;
			//	//}
			//}
			if(mGameManager->mInput->isKeyDown("KC_SPACE"))
			{
					//if(jumpCharge<2.9f)
				//	{
				//	jumpCharge = 2.9f;
				//	}

				if(cc->jump(3.75f))
				{

					mGameManager->mAudio->mEngine->play2D("../media/audio/jump_01.wav",false);
					//jumpCharge = 0.f;

				}

				/*if(cc->midAir)
				{
					if(!cc->jumping)
					{
					cc->gravityFactor-=0.05f;
					//cc->gravityFactor+=0.05f;
					}
					else
					{
						cc->fallSpeed-=0.15f;
					}
				}*/
				//spaceDown = false;
			}

			if(STEPSCALE<1.95f)
			{
				STEPSCALE+=(1.f/18000.f)*(delta*60.f*(mGameManager->difficulty+1));
			}

				if(cc->speed<20.f)
				{
					cc->speed+=(1.f/1000.f)*(delta*60.f*(mGameManager->difficulty+1));
				}

				Ogre::Vector3 tmp = mGameManager->mGfx->mMainCam->getDerivedDirection();
				if(!cc->midAir)
				{
					tmp.y = 0;
				}
				else
				{
					tmp.y*=1.8f;
					if(tmp.y>0)
					{
						//tmp.y/=2;
						if(!cc->jumping)
						{
							tmp.y = 0;
						}
						else
						{
							tmp.y/=2.f;
						}
					}
					//tmp.y/2;
				}
				tmp.normalise();
				mv+=tmp;
			

			if(cc->midAir)
			{
				Ogre::Vector3 tempv = cc->mMove+mv*0.2f;
				tempv.normalise();
				float len = cc->mMove.length();
				if(len<0.2f)
				{
					len = 0.2f;
				}
				tempv*=len;
			cc->setMove(tempv);
			}
			else
			{
			cc->setMove(mv);
			}

			// warning: may cause seizures
			//float intensity = (1.f+Ogre::Math::Sin(clock()/50.f))/2.f;
			//mGameManager->mGfx->mViewport->setBackgroundColour(Ogre::ColourValue(intensity,intensity,intensity)*mBaseColor);
			for(unsigned int i = 0;i<mGfxObjects.size();++i)
			{
				mGfxObjects[i]->update();
			}

			int oldMax = max_dist;
			if(-static_cast<int>(cc->getPosition().z)>max_dist)
			{
				max_dist = -static_cast<int>(cc->getPosition().z);
				// bonus distances
				std::vector<int> bonuses;
				bonuses.push_back(100);
				bonuses.push_back(250);
				bonuses.push_back(500);
				bonuses.push_back(750);
				bonuses.push_back(1000);
				bonuses.push_back(1500);
				bonuses.push_back(2000);
				bonuses.push_back(2500);
				bonuses.push_back(5000);
				bonuses.push_back(7500);
				bonuses.push_back(10000);
				for(unsigned int i = 0;i<bonuses.size();++i)
				{
					if(max_dist>=bonuses[i]&&oldMax<bonuses[i])
					{
						score+=multiplier*bonuses[i]*2.5f;
						setScore(Ogre::StringConverter::toString(bonuses[i]) + " meter bonus! +"+Ogre::StringConverter::toString(multiplier*bonuses[i]*2.5f),3,4.25f);
						mGameManager->mAudio->mEngine->play2D("../media/audio/pickup.wav",false);
					}
				}
			}
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("DistanceText")->setCaption("Distance: "+Ogre::StringConverter::toString(max_dist)+ " meters");


			unsigned int mins = 0;
			unsigned int secs = 0;
			unsigned int mills = 0;
			
			unsigned int totalTime = clock()-startTime;

			mins = static_cast<int>(floor(static_cast<float>(totalTime/(60.0f*CLOCKS_PER_SEC)))); totalTime = totalTime%(60*CLOCKS_PER_SEC);
			secs = static_cast<int>(floor(static_cast<float>(totalTime/(1.0f*CLOCKS_PER_SEC)))); totalTime = totalTime%(1*CLOCKS_PER_SEC);
			mills = static_cast<int>(floor(static_cast<float>(totalTime/(1.0f*10.f)))); totalTime = totalTime%(1*10);

			minsLast = mins;
			secsLast = secs;
			millsLast = mills;

			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ScoreText")->setCaption("Score: "+Ogre::StringConverter::toString(static_cast<int>(score)));


			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("TimeText")->setCaption("Time: "+Ogre::StringConverter::toString(mins)+":"+Ogre::StringConverter::toString(secs)+":"+Ogre::StringConverter::toString(mills));
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("IslesText")->setCaption("Combo: "+Ogre::StringConverter::toString(combo));
			Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("MultiText")->setCaption("Multiplier: "+Ogre::StringConverter::toString(multiplier)+"x");
			if(dur>0)
			{
				dur-=delta;
				if(dur<=0)
				{
					multiplier = (mGameManager->difficulty+1);
				}
			}
		}
	}

	Island* PlayState::createIsland(std::string mesh,float px,float py,float pz,float speed,int type)
	{
		bool foundMatch = false;
		for(unsigned int i=0;i<mInactiveIsles.size();++i)
		{
			if(mInactiveIsles[i]->object->mEntity->getMesh()->getName()==mesh)
			{
				mIsles.push_back(mInactiveIsles[i]);
				mInactiveIsles.erase(mInactiveIsles.begin()+i);
				mIsles[mIsles.size()-1]->reactivate(Ogre::Vector3(px,py,pz));
				return mIsles[mIsles.size()-1];
			}
		}
		mIsles.push_back(new Island(mGameManager,mesh,Ogre::Vector3(px,py,pz),speed,type));
		return mIsles[mIsles.size()-1];
	}

	void PlayState::generateIslands()
	{
		unsigned int stps = static_cast<unsigned int>((-cc->getPosition().z)/(20.f*STEPSCALE));
		// we stay 5 steps ahead (ish...)
		if(stps+8>nrSteps)
		{
			//std::cout<<"yep?\n";
			while(nrSteps<stps+8)
			{
				generateArrangement(nrSteps);
				++nrSteps;
			}
		}
	}

	void PlayState::generateArrangement(unsigned int step)
	{
		int lane = -1;
		while(lane<2)
		{
			int i = randInt(0,100);
			if(mGameManager->difficulty==0)
			{
				if(i<=29)
				{
					createIsland("Island_M.mesh",0+lane*20.f*STEPSCALE+randFloat(-3.f,3.f)*STEPSCALE,randFloat(-0.2f,0.9f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-3.f,3.f),2,0);
				}
				else if(i<=36)
				{
					createIsland("Island_S.mesh",-randFloat(-1.5f,-1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.6f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=46)
				{
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
				}
				else if(i<=56)
				{
					createIsland("Island_S.mesh",5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",-5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
				}
				else if(i<=58)
				{
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
				}
				else if(i<=59)
				{
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(false)
				{
					createIsland("Island_Tiny.mesh",5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",-5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
				}
				else if(i<=64)
				{
					createIsland("Island_R1.mesh",randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.7f,0.0f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=72)
				{
					createIsland("Island_l1.mesh",randFloat(-6.5f,6.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=92)
				{
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*-5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=99)
				{
					createIsland("Island_L2.mesh",-randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
			}
			else if(mGameManager->difficulty==1)
			{
				if(i<=19)
				{
					createIsland("Island_M.mesh",0+lane*20.f*STEPSCALE+randFloat(-3.f,3.f)*STEPSCALE,randFloat(-0.2f,0.9f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-3.f,3.f),2,0);
				}
				else if(i<=29)
				{
					createIsland("Island_S.mesh",-randFloat(-1.5f,-1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.6f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=39)
				{
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
				}
				else if(i<=49)
				{
					createIsland("Island_S.mesh",5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",-5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
				}
				else if(i<=54)
				{
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
				}
				else if(i<=57)
				{
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i==58)
				{
					createIsland("Island_Tiny.mesh",5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",-5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f-randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
				}
				else if(i<=63)
				{
					createIsland("Island_R1.mesh",randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.7f,0.0f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=79)
				{
					createIsland("Island_l1.mesh",randFloat(-6.5f,6.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=94)
				{
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*-5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=99)
				{
					createIsland("Island_L2.mesh",-randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
			}
			else if(mGameManager->difficulty==2)
			{
				if(i<=14)
				{
					createIsland("Island_M.mesh",0+lane*20.f*STEPSCALE+randFloat(-3.f,3.f)*STEPSCALE,randFloat(-0.2f,0.9f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-3.f,3.f),2,0);
				}
				else if(i<=34)
				{
					createIsland("Island_S.mesh",-randFloat(-1.5f,-1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.6f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=41)
				{
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
				}
				else if(i<=49)
				{
					createIsland("Island_S.mesh",5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",-5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
				}
				else if(i<=59)
				{
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
				}
				else if(i<=64)
				{
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=69)
				{
					createIsland("Island_Tiny.mesh",5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",-5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f-randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
				}
				else if(i<=74)
				{
					createIsland("Island_R1.mesh",randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.7f,0.0f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=89)
				{
					createIsland("Island_l1.mesh",randFloat(-6.5f,6.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=94)
				{
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*-5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=99)
				{
					createIsland("Island_L2.mesh",-randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
			}
			else if(mGameManager->difficulty==3)
			{
				if(i<=4)
				{
					createIsland("Island_M.mesh",0+lane*20.f*STEPSCALE+randFloat(-3.f,3.f)*STEPSCALE,randFloat(-0.2f,0.9f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-3.f,3.f),2,0);
				}
				else if(i<=19)
				{
					createIsland("Island_S.mesh",-randFloat(-1.5f,-1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.6f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=24)
				{
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
				}
				else if(i<=29)
				{
					createIsland("Island_S.mesh",5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
					createIsland("Island_S.mesh",-5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
				}
				else if(i<=49)
				{
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*-5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
				}
				else if(i<=62)
				{
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",randFloat(-3.75f,3.75f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=74)
				{
					createIsland("Island_Tiny.mesh",5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
					createIsland("Island_Tiny.mesh",-5.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f-randFloat(-3.75f,3.75f)*STEPSCALE,2,0);
				}
				else if(i<=79)
				{
					createIsland("Island_R1.mesh",randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.7f,0.0f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
				else if(i<=84)
				{
					createIsland("Island_l1.mesh",randFloat(-6.5f,6.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=89)
				{
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*-5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
					createIsland("Island_l1.mesh",randFloat(-1.5f,1.5f)+STEPSCALE*5.f*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-3.5f,3.5f)*STEPSCALE,2,0);
				}
				else if(i<=99)
				{
					createIsland("Island_L2.mesh",-randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
				}
			}
			//int i = randInt(0,10);
			//int j = randInt(0,4);
			//if(j==2&&lane!=0)
			//{
			//	lane++;
			//	continue;
			//}
			//if(i==1)
			//{
			//	createIsland("Island_S.mesh",-randFloat(-1.5f,-1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.6f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-0.5f,0.5f)*STEPSCALE,2,0);
			//	//createIsland("Island_S.mesh",randFloat(-1.5f,-1.5f)*STEPSCALE,0,static_cast<float>(step)*-20.f*STEPSCALE-randFloat(2.5f,8.5f)*STEPSCALE,2,0);
			//}
			//else if(i==2)
			//{
			//	// stacked on x
			//	createIsland("Island_S.mesh",5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
			//	createIsland("Island_S.mesh",-5*STEPSCALE+randFloat(-1.8f,1.8f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-5.2f,5.2f)*STEPSCALE,2,0);
			//	//createIsland("Island_S.mesh",randFloat(2.5f,8.5f),0,static_cast<float>(step)*-20.f+randFloat(2.5f,8.5f),2,0);
			//	//createIsland("Island_S.mesh",randFloat(2.5f,8.5f),0,static_cast<float>(step)*-20.f-randFloat(2.5f,8.5f),2,0);
			//}
			//else if(i==3)
			//{
			//	// stacked on z
			//	createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
			//	createIsland("Island_S.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f-randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
			//	//createIsland("Island_S.mesh",randFloat(2.5f,8.5f),0,static_cast<float>(step)*-20.f+randFloat(2.5f,8.5f),2,0);
			//	//createIsland("Island_S.mesh",randFloat(2.5f,8.5f),0,static_cast<float>(step)*-20.f-randFloat(2.5f,8.5f),2,0);
			//}
			//else if(i==4)
			//{
			//	//createIsland("Island_L1.mesh",-randFloat(1.5f,6.5f)*STEPSCALE+lane*20.f,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//	createIsland("Island_l1.mesh",randFloat(-6.5f,6.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-2.5f,2.5f)*STEPSCALE,2,0);
			//}
			//else if(i==5&&j==1)
			//{
			//	createIsland("Island_L2.mesh",-randFloat(-5.5f,5.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//	//createIsland("Island_l1.mesh",randFloat(1.5f,6.5f)*STEPSCALE+lane*20.f,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//}
			//else if(i==6&&j==3)
			//{
			//	createIsland("Island_R1.mesh",randFloat(1.5f,1.5f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.7f,0.0f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//	//createIsland("Island_l1.mesh",randFloat(1.5f,6.5f)*STEPSCALE+lane*20.f,randFloat(-0.2f,0.7f),static_cast<float>(step)*-20.f*STEPSCALE-randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//}
			//else if(i==7)
			//{
			//	//createIsland("Island_L1.mesh",-randFloat(1.5f,6.5f)*STEPSCALE+lane*20.f,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",randFloat(-5.2f,5.2f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE-STEPSCALE*5.f+randFloat(-1.8f,1.8f)*STEPSCALE,2,0);
			//}
			//else if(i==8)
			//{
			//	//createIsland("Island_L1.mesh",-randFloat(1.5f,6.5f)*STEPSCALE+lane*20.f,randFloat(-0.2f,0.1f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-1.5f,1.5f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*-5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",STEPSCALE*-5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
			//	createIsland("Island_Tiny.mesh",STEPSCALE*5.f+randFloat(-2.f,2.f)*STEPSCALE+lane*20.f*STEPSCALE,randFloat(-0.2f,0.65f),static_cast<float>(step)*-20.f*STEPSCALE+STEPSCALE*5.f-randFloat(-2.f,2.f)*STEPSCALE,2,0);
			//}
			//else
			//{
			//	createIsland("Island_M.mesh",0+lane*20.f*STEPSCALE+randFloat(-3.f,3.f)*STEPSCALE,randFloat(-0.2f,0.9f),static_cast<float>(step)*-20.f*STEPSCALE+randFloat(-3.f,3.f),2,0);
			//}
			lane++;
		}
		//createIsland();
	}

	void PlayState::setScore(std::string cap,unsigned int stat,float dur)
	{
		float lowestDur = 5.f;
		int lowest = 5;
		std::vector<int> possibleSlots;
		for(unsigned int i=0;i<4;++i)
		{
			if(!scores[i].displaying&&scores[i].dur<=0.f)
			{
				possibleSlots.push_back(i);
				//scores[i].setCaption(cap,stat,2.f);
				//return;
			}
			else
			{
				if(scores[i].dur<lowestDur)
				{
					lowestDur = scores[i].dur;
					lowest = i;
				}
			}
		}
		if(possibleSlots.size()>1)
		{
			scores[possibleSlots[rand()%possibleSlots.size()]].setCaption(cap,stat,dur);
			return;
		}
		else if(possibleSlots.size()>0)
		{
			scores[possibleSlots[0]].setCaption(cap,stat,dur);
			return;
		}
		if(lowest!=5)
		{
			scores[lowest].setCaption(cap,stat,dur);
		}
	}


}