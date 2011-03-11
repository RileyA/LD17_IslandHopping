#include "stdafx.h"
#include "Island.h"

#include "GameManager.h"
#include "GfxObj.h"
#include "PlayState.h"

namespace LD
{
	Island::Island(GameManager* gmgr,std::string mesh,Ogre::Vector3 pos,float speedval,unsigned int type)
	{
		pickup = 0;
		gmg = gmgr;
		object = new GfxObj(mesh);
		object->mNode->setPosition(pos);
		pobj = gmg->mPhysics->createStaticTrimesh(mesh,pos,0,0);
		readyForDelete = false;
		materialCopy = object->mEntity->getSubEntity(0)->getMaterial()->clone(object->mEntity->getName()+object->mEntity->getSubEntity(0)->getMaterialName());
		object->mEntity->setMaterial(materialCopy);
		materialCopy->setAmbient(Ogre::ColourValue(0.7,0.19,0.19));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));

		if(object->mEntity->getMesh()->getName()=="Island_Runway.mesh")
		{
			materialCopy->setAmbient(Ogre::ColourValue(0.2,0.3,0.6));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));
		}
		else
		{
			anomalize();
		}
		pobj->mObject->setUserPointer(this);
		hathBeenTroddenUpon = false;
	}

	Island::~Island()
	{

		delete object;
		if(pickup)
		{
			delete pickup;
		}
	}

	void Island::update(float delta)
	{
		if(pickup)
		{
			pickup->mNode->yaw(Ogre::Radian(Ogre::Degree(delta*90)));
			pickup->mNode->setPosition(pickup->mNode->getPosition().x,object->mNode->getPosition().y+Ogre::Math::Sin(static_cast<float>(clock())/1000.f)*0.3f,pickup->mNode->getPosition().z);

			if(pickup->mNode->getPosition().squaredDistance(gmg->mGfx->mMainCam->getDerivedPosition())<3.75f)
			{
				if(pickup->mEntity->getMesh()->getName()=="PAG_Bonus.mesh")
				{
					gmg->getState()->score+=5000*gmg->getState()->multiplier;
					gmg->getState()->setScore("How...Pretentious... (+"+Ogre::StringConverter::toString(gmg->getState()->multiplier*5000)+")",3,6.f);
					gmg->mAudio->mEngine->play2D("../media/audio/eastereggpickup.wav",false);
				}
				if(pickup->mEntity->getMesh()->getName()=="Twilight_Bonus.mesh")
				{
					gmg->getState()->score+=4000*gmg->getState()->multiplier;
					gmg->getState()->setScore("LD18: Twilight Fandom! (+"+Ogre::StringConverter::toString(4000*gmg->getState()->multiplier)+")",3,6.f);
					gmg->mAudio->mEngine->play2D("../media/audio/eastereggpickup.wav",false);
				}
				if(pickup->mEntity->getMesh()->getName()=="48_Bonus.mesh")
				{
					gmg->getState()->setScore("You're not getting those 48hrs back... (+0)",3,6.f);
					gmg->mAudio->mEngine->play2D("../media/audio/eastereggpickup.wav",false);
				}

				if(pickup->mEntity->getMesh()->getName()=="750_Bonus.mesh")
				{
					gmg->getState()->score+=750*gmg->getState()->multiplier;
					gmg->getState()->setScore("+"+Ogre::StringConverter::toString(750*gmg->getState()->multiplier)+" Bonus Points!",3,3.5f);
					gmg->mAudio->mEngine->play2D("../media/audio/pickup.wav",false);
				}
				if(pickup->mEntity->getMesh()->getName()=="1500_Bonus.mesh")
				{
					gmg->getState()->score+=1500*gmg->getState()->multiplier;
					gmg->getState()->setScore("+"+Ogre::StringConverter::toString(1500*gmg->getState()->multiplier)+" Bonus Points!",3,3.5f);
					gmg->mAudio->mEngine->play2D("../media/audio/pickup.wav",false);
				}
				if(pickup->mEntity->getMesh()->getName()=="3000_Bonus.mesh")
				{
					gmg->getState()->score+=3000*gmg->getState()->multiplier;
					gmg->getState()->setScore("+"+Ogre::StringConverter::toString(3000*gmg->getState()->multiplier)+" Bonus Points!",3,3.5f);
					gmg->mAudio->mEngine->play2D("../media/audio/pickup.wav",false);
				}

				if(pickup->mEntity->getMesh()->getName()=="x2_Bonus.mesh")
				{
					gmg->getState()->multiplier =(static_cast<float>(gmg->difficulty+1))*2;
					gmg->getState()->dur = 20.f;
					gmg->getState()->setScore("2x score multiplier for 20 seconds.",3,6.0f);
					gmg->mAudio->mEngine->play2D("../media/audio/multiplier.wav",false);
				}

				if(pickup->mEntity->getMesh()->getName()=="d2_Bonus.mesh")
				{
					gmg->getState()->multiplier =(static_cast<float>(gmg->difficulty+1))/2;
					gmg->getState()->dur = 20.f;
					gmg->getState()->setScore("0.5x score multiplier for 20 seconds.",2,6.0f);
					gmg->mAudio->mEngine->play2D("../media/audio/multiplier_negative.wav",false);
				}

				if(pickup->mEntity->getMesh()->getName()=="x4_Bonus.mesh")
				{
					gmg->getState()->multiplier =(static_cast<float>(gmg->difficulty+1))*4;
					gmg->getState()->dur = 15.f;
					gmg->getState()->setScore("4x score multiplier for 15 seconds.",3,6.0f);
					gmg->mAudio->mEngine->play2D("../media/audio/multiplier.wav",false);
				}


				//std::cout<<""
				delete pickup;
				pickup = 0;


			}
		}
	}

	void Island::landedOn()
	{
		if(!hathBeenTroddenUpon)
		{
			hathBeenTroddenUpon = true;
			if(object->mEntity->getMesh()->getName()!="Island_Runway.mesh")
			{
		float m = gmg->getState()->multiplier;

				//snd->setVolume(0.6f);
				materialCopy->setAmbient(Ogre::ColourValue(0.2,0.3,0.6));
				if(tp==0)
				{
				gmg->mAudio->mEngine->play2D("../media/audio/hit_green.wav",false);//->setVolume(0.6f);

					gmg->getState()->score+=500*m+250*m*(gmg->getState()->combo-1);
					gmg->getState()->combo++;
					if(gmg->getState()->combo>1)
					{
						gmg->getState()->setScore("combo! "+Ogre::StringConverter::toString(500*m)+" + "+Ogre::StringConverter::toString(250*m)+"x"+Ogre::StringConverter::toString(gmg->getState()->combo-1)+" = "+Ogre::StringConverter::toString(500*m+250*m*(gmg->getState()->combo-1)),1);
					}
					else
					{
						gmg->getState()->setScore("+"+Ogre::StringConverter::toString(500*m),1);
					}
				}
				else if(tp==1)
				{
				gmg->mAudio->mEngine->play2D("../media/audio/hit_01.wav",false);//->setVolume(0.6f);

					gmg->getState()->score-=500*m;
					if(gmg->getState()->combo>1)
					{
						gmg->getState()->setScore("-"+Ogre::StringConverter::toString(500*m)+" combo broken!",2);	
					}
					else
					{
						gmg->getState()->setScore("-"+Ogre::StringConverter::toString(500*m),2);
					}
					gmg->getState()->combo = 0;

				}
				else
				{
				gmg->mAudio->mEngine->play2D("../media/audio/hit_grey.wav",false);//->setVolume(0.6f);

					gmg->getState()->score+=50*m;
					if(gmg->getState()->combo>1)
					{
						gmg->getState()->setScore("+"+Ogre::StringConverter::toString(50*m)+" combo broken!",2);
					}
					else
					{
						gmg->getState()->setScore("+"+Ogre::StringConverter::toString(50*m),0);
					}
					gmg->getState()->combo = 0;

				}
				gmg->getState()->nrIslands++;
			}
		}
		//randFloat(0,1),randFloat(0,1),randFloat(0,1)));
	}

	void Island::setVisible(bool visible)
	{
		if(!visible)
		{
			object->mNode->setVisible(false);
			pobj->mObject->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(0,-50,0)));
			hathBeenTroddenUpon = false;
			materialCopy->setAmbient(Ogre::ColourValue(0.7,0.19,0.19));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));

			if(object->mEntity->getMesh()->getName()=="Island_Runway.mesh")
			{
				materialCopy->setAmbient(Ogre::ColourValue(0.2,0.3,0.6));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));
			}

			if(pickup)
			{
			delete pickup;
			pickup = 0;
			}

		}
	}

	void Island::reactivate(Ogre::Vector3 pos)
	{
		object->mNode->setPosition(pos);
		pobj->mObject->setWorldTransform(btTransform(btQuaternion::getIdentity(),Vector3_bt(pos)));
		object->mNode->setVisible(true);
		hathBeenTroddenUpon = false;
		materialCopy->setAmbient(Ogre::ColourValue(0.7,0.19,0.19));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));

		if(object->mEntity->getMesh()->getName()=="Island_Runway.mesh")
		{
			materialCopy->setAmbient(Ogre::ColourValue(0.2,0.3,0.6));//randFloat(0,1),randFloat(0,1),randFloat(0,1)));
		}
		else
		{
			anomalize();
		}
	}


	void Island::anomalize()
	{
		Ogre::ColourValue color;
		int i = randInt(0,6);
		int j = randInt(0,100); 
		// determine type
		if(j<3)
		{
			tp = 3;
			int k = randInt(0,100); 
			color = Ogre::ColourValue(0.9,0.5,0.3);

			if(k==0)
			{
				pickup = new GfxObj("Twilight_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
			}
			else if(k==1)
			{
				//color = Ogre::ColourValue(0.9,0.5,0.3);
				pickup = new GfxObj("PAG_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
			}
			else if(k==2)
			{
				//color = Ogre::ColourValue(0.9,0.5,0.3);
				pickup = new GfxObj("48_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
			}
			else if(k<8)
			{
				pickup = new GfxObj("x4_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
			else if(k<18)
			{
				pickup = new GfxObj("x2_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
			else if(k<28)
			{
				pickup = new GfxObj("d2_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
			else if(k<48)
			{
				pickup = new GfxObj("3000_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
			else if(k<73)
			{
				pickup = new GfxObj("1500_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
			else
			{
				pickup = new GfxObj("750_Bonus.mesh");
				pickup->mNode->setPosition(object->mNode->getPosition());
				// do stuff
			}
		}
		else if(i<=1)
		{
			tp = 0;
			color = Ogre::ColourValue(0.3,0.7,0.4);
		}
		else if(i<=2)
		{
			tp = 1;
			color = Ogre::ColourValue(0.8,0.3,0.4);
		}
		else
		{
			tp = 2;
			color = Ogre::ColourValue(0.4,0.4,0.4);
		}

		materialCopy->setAmbient(color);

	}


	
}