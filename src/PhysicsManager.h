#pragma once
#include "stdafx.h"
#include "Utils.h"
#include "CharacterController.h"

namespace LD
{
	class PhysicsObject
	{
	public:

		PhysicsObject(btCollisionObject* obj,Ogre::Vector3 posi = Ogre::Vector3(0,0,0),Ogre::Quaternion orie = Ogre::Quaternion::IDENTITY)
			:mObject(obj),mDynamic(0),deleteMe(0)
		{
			pos[0] = Vector3_bt(posi);
			pos[1] = Vector3_bt(posi);
			ori[0] = Quat_bt(orie);
			ori[1] = Quat_bt(orie);
		}

		PhysicsObject(btRigidBody* robj,Ogre::Vector3 posi = Ogre::Vector3(0,0,0),Ogre::Quaternion orie = Ogre::Quaternion::IDENTITY)
			:mRigidObject(robj),mObject(robj),mDynamic(1),deleteMe(0)
		{
			pos[0] = Vector3_bt(posi);
			pos[1] = Vector3_bt(posi);
			ori[0] = Quat_bt(orie);
			ori[1] = Quat_bt(orie);
		}

		~PhysicsObject()
		{
			if(mDynamic)
			{
				delete mRigidObject;
			}
			else
			{
				delete mObject;
			}
			// we just let the dynamics world delete stuff on its own...
		}

		void update(float interpolation,bool newFrame)
		{
			if(mDynamic)
			{

				if(newFrame)
				{
					pos[1] = pos[0];
					pos[0] = mRigidObject->getCenterOfMassPosition();
					ori[1] = ori[0];
					ori[0] = mRigidObject->getOrientation();
				}
				mInterpolation = interpolation;
			}
		}

		Ogre::Vector3 getPosition()
		{
			return bt_Vector3(pos[1]*(1.f-mInterpolation)+pos[0]*(mInterpolation));
		}

		Ogre::Quaternion getOrientation()
		{
			return bt_Quat(ori[1].slerp(ori[0],mInterpolation));//bt_Quaternion(btQuaternion::slerp(ori[0],ori[1],mInterpolation));
		}

		bool mDynamic;
		// cause I'm too lazy to cast between 'em...
		btCollisionObject* mObject;
		btRigidBody* mRigidObject;
		btVector3 pos[2];
		btQuaternion ori[2];
		float mInterpolation;

		bool deleteMe;
	};

	class PhysicsManager
	{
	public:

		PhysicsManager();
		~PhysicsManager();

		void init();
		void deinit();
		void update(float delta);

		// clear out cached physics collision shapes
		void clearCache();

		// Some functions for generating physics meshes, mostly pasted in from another project...
		PhysicsObject* createStaticTrimesh(std::string meshname,Ogre::Vector3 pos=Ogre::Vector3(0,0,0),float _restitution=0.5f,float _friction=0.5f);
		PhysicsObject* createConvexHull(std::string meshname,Ogre::Vector3 pos=Ogre::Vector3(0,0,0),Ogre::Vector3 scale=Ogre::Vector3(1,1,1),float _mass=20.f,float _restitution=0.5f,float _friction=0.5f);
		CharacterController* createCharacterController(std::string meshname,Ogre::Vector3 pos=Ogre::Vector3(0,0,0),Ogre::Vector3 scale=Ogre::Vector3(1,1,1),float _mass=20.f,float _restitution=0.5f,float _friction=0.5f);
		//CharacterController* createCharacterControllerCap(Ogre::Vector3 pos=Ogre::Vector3(0,0,0),Ogre::Vector3 scale=Ogre::Vector3(1,1,1),float _mass=20.f,float _restitution=0.5f,float _friction=0.5f);
		PhysicsObject* createCube(Ogre::Vector3 scale,Ogre::Vector3 pos,float _mass=20.f,float _restitution=0.5f,float _friction=0.5f);
		CharacterController* PhysicsManager::createCCTCap(Ogre::Vector3 scale,Ogre::Vector3 pos,float _mass,float _restitution,float _friction);

	//[un]protected:

		bool started;
		bool mInitialized;
		float mTimeStep;
		float mAccumulation;
		float mInterpolation;

		btDynamicsWorld* mDynamicsWorld;

		// all the random bullet bits...
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		btCollisionDispatcher* mDispatcher;
		btDbvtBroadphase* mBroadphase;
		btSequentialImpulseConstraintSolver* mSolver;

		std::vector<PhysicsObject*> mObjects;
		std::vector<CharacterController*> mControllers;

		// Cache the collision shapes for reuse...
		std::map<std::string,btCollisionShape*> mShapes;

	};

	#define BIT(x) (1<<(x))
	enum CollisionTypes 
	{
		COL_NOTHING = 0, //Collide with nothing
		COL_STATICS = BIT(1), //Collide with default static trimeshes
		COL_DYNAMICS = BIT(2), //Collide with default dynamic stuff
		COL_KINEMATICS = BIT(3) //Collide with default kinematics
	};

	struct	OverlapResultCallback2 : public btCollisionWorld::ContactResultCallback
	{
		unsigned int hits;

		OverlapResultCallback2()
		{
			hits = 0;
			m_collisionFilterGroup = COL_STATICS;//???
			m_collisionFilterMask = COL_STATICS|COL_DYNAMICS;
		}

		btCollisionObject* obj0;
		btCollisionObject* obj1;

		virtual	btScalar addSingleResult(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
		{
			if(cp.getDistance()<=0)
			{
			++hits;
			obj0 = const_cast<btCollisionObject*>(colObj0);
			obj1 = const_cast<btCollisionObject*>(colObj1);
			}
			return 0.f;
		}
	};
}