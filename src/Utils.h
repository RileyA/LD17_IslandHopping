#ifndef UTILITIES_H
#define UTILITIES_H
#include "stdafx.h"

namespace LD
{
	// basic conversion stuff

	class GameManager;
	class PlayState;

	btVector3 Vector3_bt(const Ogre::Vector3& v);
	Ogre::Vector3 bt_Vector3(const btVector3& v);
	btQuaternion Quat_bt(const Ogre::Quaternion& q);
	Ogre::Quaternion bt_Quat(const btQuaternion& q);
	double randDouble(double low, double high);
	float randFloat(float low, float high);
	int randInt(int low, int high);
	void seedRand(unsigned int seed);


}

	// ugly ugly global, but for laziness' sake it works
	namespace UglyHack
	{
		// ugly ugly _UGLY_ global, but it's an easy way of giving chaiscript access from any function
		extern LD::GameManager* gmgr;// = 0;
		extern LD::PlayState* pst;// = 0;
		 LD::GameManager* getMgr();
		 LD::PlayState* getPState();
		 void createIsle(std::string mesh,float x,float y,float z,float spd,unsigned int type);
	}



#endif