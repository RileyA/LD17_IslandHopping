#pragma once
#include "stdafx.h"

namespace LD
{
	class AudioManager
	{
	public:

		AudioManager();
		~AudioManager();

		void init();
		void deinit();

		void update();

	//[un]protected:

		bool mInitialized;
		irrklang::ISoundEngine* mEngine;
	};
}