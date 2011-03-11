#pragma once
#include "stdafx.h"

namespace LD
{
	class GfxObj
	{
	public:
		GfxObj(std::string meshname,PhysicsObject* obj=0)
		{
			mSmgr = Ogre::Root::getSingletonPtr()->getSceneManager("mSceneManager");
			mEntity = mSmgr->createEntity(meshname);
			mNode = mSmgr->createSceneNode();
			mNode->attachObject(mEntity);
			mSmgr->getRootSceneNode()->addChild(mNode);
			mObj = obj;
		}
		~GfxObj()
		{
			mObj = 0;
			mNode->detachAllObjects();
			mSmgr->destroyEntity(mEntity);
			mSmgr->destroySceneNode(mNode);
		}
		void update()
		{
			if(mObj)
			{
				mNode->setPosition(mObj->getPosition());
				mNode->setOrientation(mObj->getOrientation());
			}
		}

		Ogre::Entity* mEntity;
		Ogre::SceneNode* mNode;
		PhysicsObject* mObj;
		Ogre::SceneManager* mSmgr;
	};
}