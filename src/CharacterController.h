#pragma once
#include "stdafx.h"

namespace LD
{
	class PhysicsManager;

	class CharacterController
	{
	public:

		CharacterController(PhysicsManager* pmgr,btRigidBody* rb,Ogre::Vector3 pos);
		~CharacterController();

		void init(Ogre::Vector3 position);

		void update(float delta,bool newFrame,float interpolation);

		void setMove(Ogre::Vector3 moveV);

		Ogre::Vector3 getPosition();

		void setVeloc();

		void setSpeed(float spd);

		bool jump(float power);

		bool isMidair();

		void setPosition(Ogre::Vector3 npos);

		float speed;
		bool midAir;
		bool midAirLast;
		Ogre::Vector3 mMove;
		btRigidBody* rObj;
		PhysicsManager* mPmgr;
		Ogre::Vector3 mPos[2];
		float mInterpolation;
		btGeneric6DofConstraint* lockRot;

		float gravityFactor;
		bool jumping;

		float playerVertSpd;
		float playerSpd;
		float fallSpeed;

	};
}