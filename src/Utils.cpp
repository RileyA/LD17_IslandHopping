#include "stdafx.h"
#include "GameManager.h"
#include "PlayState.h"

namespace LD
{
	btVector3 Vector3_bt(const Ogre::Vector3& v)
	{
		return btVector3(v.x,v.y,v.z);
	}
	Ogre::Vector3 bt_Vector3(const btVector3& v)
	{
		return Ogre::Vector3(v.x(),v.y(),v.z());
	}
	btQuaternion Quat_bt(const Ogre::Quaternion& q)
	{
		return btQuaternion(q.x,q.y,q.z,q.w);
	}
	Ogre::Quaternion bt_Quat(const btQuaternion& q)
	{
		return Ogre::Quaternion(q.w(),q.x(),q.y(),q.z());
	}

	void seedRand(unsigned int seed)
	{
		srand(seed);
	}

	double randDouble(double low, double high)
	{
		double temp;

		if (low > high)
		{
			temp = low;
			low = high;
			high = temp;
		}

		temp = (rand() / (static_cast<double>(RAND_MAX) + 1.0))
		* (high - low) + low;
		return temp;
	}

	float randFloat(float low, float high)
	{
		float temp;

		if (low > high)
		{
			temp = low;
			low = high;
			high = temp;
		}

		temp = (rand() / (static_cast<float>(RAND_MAX) + 1.0))
		* (high - low) + low;
		return temp;
	}

	int randInt(int low, int high)
	{
		int temp;

		if (low > high)
		{
			temp = low;
			low = high;
			high = temp;
		}
		temp = (rand() / (static_cast<int>(RAND_MAX) + 1.0))
		* (high - low) + low;
		return temp;
	}


}

namespace UglyHack
{
	// ugly ugly _UGLY_ global, but it's an easy way of giving chaiscript access from any function
	LD::GameManager* gmgr;
	LD::PlayState* pst;

	LD::GameManager* getMgr()
	{
		return gmgr;
	}

	LD::PlayState* getPState()
	{
		return pst;
	}

	void createIsle(std::string mesh,float x,float y,float z,float spd,unsigned int type)
	{
		pst->createIsland(mesh,x,y,z,spd,type);
	}

}
