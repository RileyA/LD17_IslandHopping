#include "stdafx.h"
#include "CharacterController.h"
#include "PhysicsManager.h"
#include "island.h"

namespace LD
{
	CharacterController::CharacterController(PhysicsManager* pmgr,btRigidBody* rb,Ogre::Vector3 pos)
	{
		mPmgr = pmgr;
		rObj = rb;
		rObj->setGravity(btVector3(0,0,0));
		rObj->setRestitution(0.f);
		rObj->setFriction(0.f);
		rObj->setWorldTransform(btTransform(btQuaternion::getIdentity(),Vector3_bt(pos)));
		lockRot = new btGeneric6DofConstraint(*rObj,btTransform::getIdentity(),true);
		lockRot->setLinearLowerLimit(btVector3(10,10,10));
		lockRot->setLinearUpperLimit(btVector3(-10,-10,-10));
		// lock!
		lockRot->setAngularLowerLimit(btVector3(0,0,0));
		lockRot->setAngularUpperLimit(btVector3(0,0,0));
		mPmgr->mDynamicsWorld->addConstraint(lockRot,true);
		mPos[0] = pos;
		mPos[1] = pos;
		speed = 6.f;
		jumping = false;

		playerVertSpd = 0.f;
		playerSpd = 0.f;

		gravityFactor = 1.f;
		fallSpeed = 0.f;

		midAir = false;
		midAirLast = false;
		//mPos[1] = pos;
	}

	CharacterController::~CharacterController()
	{
		rObj->removeConstraintRef(lockRot);
		mPmgr->mDynamicsWorld->removeConstraint(lockRot);
		delete lockRot;
		lockRot = 0;
	}

	void CharacterController::update(float delta,bool newFrame,float interpolation)
	{

		if(newFrame)
		{
			isMidair();

			//setVeloc();

			//btCollisionWorld::ClosestConvexResultCallback closeCall(Vector3_bt(mPos[0]),Vector3_bt(mPos[0]+mMove*0.05f));
			//mPmgr->mDynamicsWorld->convexSweepTest(dynamic_cast<btConvexShape*>(rObj->getCollisionShape()),btTransform(btQuaternion(0,0,0,1),Vector3_bt(mPos[0])),btTransform(btQuaternion(0,0,0,1),Vector3_bt(mPos[0]+mMove*0.05f)),closeCall);
			
		// move it

		//rObj->applyCentralImpulse(Vector3_bt(mMove*200));

			if(gravityFactor!=1)
			{
				gravityFactor+=(10.f/100.f);
				if(gravityFactor>1.f)
				{
					gravityFactor = 1.f;
				}
			}
			else
			{
				jumping = false;
			}

			mPos[1] = mPos[0];
			mPos[0] = bt_Vector3(rObj->getCenterOfMassPosition());

			//std::cout<<"Speed = "<<mPos[0].distance(mPos[1])<<"\n";
			
			float _playerSpd = Ogre::Vector3(mPos[0].x,0,mPos[0].z).distance(Ogre::Vector3(mPos[1].x,0,mPos[1].z));
			/*if(!midAir)
			{
				if(_playerSpd>=playerSpd)
				{

						speed+=0.02f;
						//std::cout<<"Accelerating..."<< speed<<"\n";
						if(speed>=7.75f)
						{
							speed = 7.75f;
						}
					
				}
				else
				{
					speed = 4.f;
				}
			}*/
			if(speed<6.f)
			{
				speed = 6.f;
			}
			float _playerVertSpd = Ogre::Vector3(0,mPos[0].y,0).distance(Ogre::Vector3(0,mPos[1].y,0));
			if(_playerVertSpd>playerVertSpd*0.95f)
			{
				if(gravityFactor>=0)
				{
					fallSpeed+=0.03f;
					if(fallSpeed>20.f)
					{
						fallSpeed = 20.f;
					}
				}
			}
			else
			{
				fallSpeed = 1.f;
			}

			if(!midAir)
			{
				fallSpeed = 0.f;
			}

			playerSpd = _playerSpd;
			playerVertSpd = _playerVertSpd;
			//if(closeCall.m_closestHitFraction<0.99f)
			//{
			//	Ogre::Vector3 svect = (mMove.dotProduct(bt_Vector3(closeCall.m_hitNormalWorld))*bt_Vector3(closeCall.m_hitNormalWorld));//
				//svect.normalise();
				//btCollisionWorld::ClosestConvexResultCallback closeCall(Vector3_bt(mPos[0]),Vector3_bt(mPos[0]+svect*0.05f));
				//mPmgr->mDynamicsWorld->convexSweepTest(dynamic_cast<btConvexShape*>(rObj->getCollisionShape()),btTransform(btQuaternion(0,0,0,1),Vector3_bt(mPos[0])),btTransform(btQuaternion(0,0,0,1),Vector3_bt(mPos[0]+svect*0.05f)),closeCall);
			//}

		}
		else if(!midAir)
		{
			mMove = Ogre::Vector3::ZERO;
		}
		else
		{
			//mMove*=0.999f;
		}
			setVeloc();

		mInterpolation = interpolation;

	}

	void CharacterController::setMove(Ogre::Vector3 moveV)
	{
		// add some limitations, speed stuff here...
		mMove = moveV;
	}

	Ogre::Vector3 CharacterController::getPosition()
	{
		return mPos[0]*mInterpolation+mPos[1]*(1-mInterpolation);
		//return bt_Vector3
	}


	void CharacterController::setVeloc()
	{
			rObj->activate();

		if(!mMove.isZeroLength())
		{
			//rObj->setDamping(0.f,0.f);
		}
		else
		{
			//rObj->setDamping(5.f,0.f);
		}

		rObj->setLinearVelocity(Vector3_bt(mMove*3.f*speed+Ogre::Vector3(0,-4.f,0)*gravityFactor*fallSpeed));
	}

	void CharacterController::setSpeed(float spd)
	{
		speed = spd;
	}

	bool CharacterController::jump(float power)
	{
		if(!midAir)
		{
			gravityFactor = -power;
			jumping = true;
			fallSpeed = 1.f;
			return true;
		}
		return false;
	}

	bool CharacterController::isMidair()
	{
		midAirLast = midAir;
		OverlapResultCallback2 overlapCall = OverlapResultCallback2();
		overlapCall.m_collisionFilterGroup = COL_STATICS;
		overlapCall.m_collisionFilterMask = COL_STATICS|COL_DYNAMICS;
		btBoxShape sphsh = btBoxShape(btVector3(0.35f,0.375f,0.35f));
		btCollisionObject* bcobj = new btCollisionObject();
		bcobj->setUserPointer(NULL);
		btVector3 boxPosition = Vector3_bt(mPos[0]-Ogre::Vector3(0,0.8f,0));
		bcobj->setWorldTransform(btTransform(btQuaternion::getIdentity(),boxPosition));
		bcobj->setCollisionShape(&sphsh);
		mPmgr->mDynamicsWorld->contactTest(bcobj,overlapCall);
		delete bcobj;

		if(overlapCall.hits>0)
		{
			midAir = false;
			//if(overlapCall.obj0)
			//{
			//	if(overlapCall.obj0->getUserPointer())
			//	{
			//		static_cast<Island*>(overlapCall.obj0->getUserPointer())->landedOn();
			//	}
			//}
			if(overlapCall.obj1)
			{
				if(overlapCall.obj1->getUserPointer())
				{
					static_cast<Island*>(overlapCall.obj1->getUserPointer())->landedOn();
				}
			}
			return false;
		}
		else
		{
			midAir = true;
			return true;
		}
	}

	void CharacterController::setPosition(Ogre::Vector3 npos)
	{
		mPos[0] = npos;
		mPos[1] = npos;
		rObj->setWorldTransform(btTransform(btQuaternion::getIdentity(),Vector3_bt(npos)));
	}

}