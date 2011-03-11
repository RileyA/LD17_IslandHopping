#include "stdafx.h"
#include "AudioManager.h"

namespace LD
{
	AudioManager::AudioManager()
		:mInitialized(0),mEngine(0)
	{}

	AudioManager::~AudioManager()
	{
		deinit();
	}

	void AudioManager::update()
	{
		// do stuff
	}

	void AudioManager::init()
	{
		mEngine = irrklang::createIrrKlangDevice();

		if (!mEngine)
		{
			printf("Could not startup audio engine!\n");
			return;
		}

		mInitialized = true;
	}

	void AudioManager::deinit()
	{
		if(mInitialized)
		{
			mEngine->drop();
			mInitialized = false;
		}
	}
}