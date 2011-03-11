#include "stdafx.h"
#include "GameManager.h"
#include "PlayState.h"

namespace LD
{
	GameManager::GameManager()
		:mInitialized(0),mQuit(0),mEndState(0),mAudio(0)
	{

	}
	GameManager::~GameManager()
	{
		deinit();
	}

	// basic setup/update stuff
	void GameManager::init()
	{
		seedRand(time(0));
		std::cout<<"starting up game...\n";
		mAudio = new AudioManager();
		mAudio->init();
		mInput = new InputManager();

		mPhysics = new PhysicsManager();

		mGfx = new GraphicsManager();
		mGfx->init(mInput);

		size_t handle = 0;
		mGfx->mWindow->getCustomAttribute("WINDOW", &handle);

		mInput->initialize(handle);
		mInput->setWindowSize(static_cast<unsigned int>(mGfx->mWindow->getWidth()),static_cast<unsigned int>(mGfx->mWindow->getHeight()));

		//mChai.add(chaiscript::fun(static_cast<std::string (*)(int)>(&ScriptManager::toString)), "toString");
		//mChai.add(chaiscript::fun(static_cast<std::string (*)(float)>(&ScriptManager::toString)), "toString");
		//mChai.add(chaiscript::fun(static_cast<std::string (*)(bool)>(&ScriptManager::toString)), "toString");
		//mChai.add(chaiscript::fun(static_cast<std::string (*)(std::string)>(&ScriptManager::toString)), "toString");
		//mChai.add(chaiscript::fun(static_cast<std::string (*)(char)>(&ScriptManager::toString)), "toString");
		//mChai.add(chaiscript::fun(&LD::seedRand), "seedRand");
		//mChai.add(chaiscript::fun(&LD::randFloat), "randFloat");
		//mChai.add(chaiscript::fun(&UglyHack::getMgr), "getMgr");
		//mChai.add(chaiscript::fun(&UglyHack::createIsle), "createIsle");

		//mChai.add(chaiscript::user_type<PlayState>(), "PlayState");
		//mChai.add(chaiscript::fun(&PlayState::createIsland), "createIsland");

		//mChai.add(chaiscript::user_type<GameManager>(), "GameManager");
		//mChai.add(chaiscript::fun(&GameManager::getState), "getState");

		mInitialized = true;
	}

	void GameManager::deinit()
	{
		if(mInitialized)
		{
			std::cout<<"shutting down game...\n";

			if(mAudio)
			{
				std::cout<<"shutting down audio...\n";
				delete mAudio;
				mAudio = 0;
			}
			if(mInput)
			{
				delete mInput;
				mInput = 0;
			}
			if(mPhysics)
			{
				delete mPhysics;
				mPhysics = 0;
			}
			if(mGfx)
			{
				std::cout<<"shutting down graphics...\n";
				delete mGfx;
				mGfx = 0;
			}

			mInitialized = false;
		}
	}
	void GameManager::go()
	{
		bool endDown = false;
		mAudio->mEngine->play2D("../media/audio/music.ogg",true);

		while(!mStates.empty())
		{
			mStates[0]->init();

			float time = static_cast<float>(clock())/1000.f;
			float lastTime = time;
			float deltaTime = 0.f;
			mEndState = 0;
			mStates[0]->mEndState = 0;

			std::vector<double> mFrameTimes;

			while(!mStates[0]->mEndState&&!mEndState&&!mQuit)
			{
				time = static_cast<float>(clock())/1000.f;
				deltaTime = time - lastTime;
				lastTime = time;

				mFrameTimes.push_back(deltaTime);

				double mSmoothedDeltaTime;

				double total = 0;
				unsigned int nrFr = 0;
				double smP = 0.125;// the smoothing period .1 of a second

				bool dn = false;
				while(!dn)
				{
					total = 0;
					nrFr = 0;
					for(unsigned int p = 0;p<mFrameTimes.size();++p)
					{
						total+=mFrameTimes[p];
						++nrFr;
					}
					if(total>smP)
					{
						std::vector<double>::iterator iter = mFrameTimes.begin();
						mFrameTimes.erase(iter);
					}
					else
					{
						dn = true;
						break;
					}
				}

				if(nrFr>1)
				{
					mSmoothedDeltaTime = total/nrFr;
				}
				else
				{
					mSmoothedDeltaTime = deltaTime;
				}

				deltaTime = static_cast<float>(mSmoothedDeltaTime);


				mInput->update();
				mGfx->update();
				mAudio->update();
				mPhysics->update(deltaTime);
				mStates[0]->update(deltaTime);

				/*if(mInput->isKeyDown("KC_A"))
				{
					mAudio->mEngine->play2D("../media/audio/coin_01.wav",false);
				}*/

				if(mInput->isKeyDown("KC_END")&&!endDown)
				{
					mEndState = true;
					endDown = true;
				}
				else if(!mInput->isKeyDown("KC_END"))
				{
					endDown = false;
				}

				//if(mInput->isKeyDown("KC_ESCAPE"))
				//{
				//	mQuit = true;
				//}
			}

			mStates[0]->deinit();

			if(mQuit)
			{
				break;
			}
			else
			{
				mStates.erase(mStates.begin());
			}
		}
	}

	void GameManager::addState(GameState* state)
	{
		mStates.push_back(state);
	}
	void GameManager::endCurrentState()
	{
		mEndState = true;
	}
	void GameManager::_forceQuit()
	{
		mQuit = true;
	}

	PlayState* GameManager::getState()
	{
		if(mStates.size()>1)
		{
			return dynamic_cast<PlayState*>(mStates[1]);
		}
		else
		{
			return dynamic_cast<PlayState*>(mStates[0]);
		}
	}
}