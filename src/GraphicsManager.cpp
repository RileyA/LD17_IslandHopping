#include "stdafx.h"

#include "GraphicsManager.h"

namespace LD
{
	GraphicsManager::GraphicsManager()
		:mInitialized(0),mRoot(0),mViewport(0),mWindow(0),mMainCam(0),mSceneManager(0),canLook(1)
	{
		
	}
	GraphicsManager::~GraphicsManager()
	{
		deinit();
	}
	void GraphicsManager::init(InputManager* input)
	{

		std::ifstream file;
		file.open("lookSensitivity.txt");
		if(file.is_open())
		{
			file>>mLookSensitivity;
			file.close();
		}
		else
		{
			mLookSensitivity = 1.f;
		}

		mInput = input;

		// load the appropriate plugins...
		#ifdef _DEBUG
			mRoot = new Ogre::Root("plugins_d.cfg");
		#else
			mRoot = new Ogre::Root("plugins.cfg");
		#endif

		// load up the old config (if possible) then run the dialogue
        mRoot->restoreConfig();
		if(mRoot->showConfigDialog())
		{
			mWindow = mRoot->initialise(true,"Island Hopping - Praetor LD17");
		}
		else
		{
			throw std::runtime_error(std::string("EXCEPTION: Config Dialogue Cancelled."));//QE::Exception(String("EXCEPTION: Config Dialogue Cancelled."));
		}

		mSceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC,"mSceneManager");


		// Set up the main camera
		mMainCam = mSceneManager->createCamera("mMainCam");
		mMainCam->setPosition(Ogre::Vector3(0,0,0));
		mMainCam->setDirection(Ogre::Vector3(0,0,-1));
		mMainCam->setFarClipDistance(300);
		mMainCam->setNearClipDistance(0.01f);
		mMainCam->setFOVy(Ogre::Radian(Ogre::Degree(65)));

		// viewport setup
		mViewport = mWindow->addViewport(mMainCam);
		mViewport->setBackgroundColour(Ogre::ColourValue(0.6f,0.6f,1));
		mMainCam->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

		mCameraNode = mSceneManager->createSceneNode("DefaultCameraManager_CameraNode");
		mCameraRollNode = mSceneManager->createSceneNode("DefaultCameraManager_CameraRollNode");
		mCameraPitchNode = mSceneManager->createSceneNode("DefaultCameraManager_CameraPitchNode");
		mCameraYawNode = mSceneManager->createSceneNode("DefaultCameraManager_CameraYawNode");
		mCameraPosNode = mSceneManager->createSceneNode("DefaultCameraManager_CameraPosNode");
		mCameraPosNode->addChild(mCameraRollNode);
		mCameraRollNode->addChild(mCameraYawNode);
		mCameraYawNode->addChild(mCameraPitchNode);
		mCameraPitchNode->addChild(mCameraNode);
		mSceneManager->getRootSceneNode()->addChild(mCameraPosNode);

		mCameraNode->attachObject(mMainCam);

		mPitchLimitUp = 80.f;
		mPitchLimitDown = 80.f;
		mCurrentPitch = 0.0f;

		camLook = false;

		mSceneManager->setFog(Ogre::FOG_LINEAR,Ogre::ColourValue(0,0,0.03f),0.001f,2.f,40.f);

		// resource locations
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/models","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/textures","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/audio","FileSystem","General");
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		Ogre::MaterialManager::getSingletonPtr()->setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingletonPtr()->setDefaultAnisotropy(16);

		Ogre::OverlayManager::getSingletonPtr()->getByName("POSTMORTEM")->hide();

		mInitialized = true;
	}
	void GraphicsManager::deinit()
	{
		if(mInitialized)
		{
			if(mRoot)
			{
				delete mRoot;
				mRoot = 0;
			}
			mViewport = 0;
			mWindow = 0;
			mMainCam = 0;
			mSceneManager = 0;
			mInitialized = false;
		}
	}
	void GraphicsManager::update()
	{
		if(camLook&&canLook)
		{
			const OIS::MouseState &ms = mInput->getOISManager()->getMouse()->getMouseState();

            float sensitivity = 0.016667*mLookSensitivity;

			                    mMouseSmoothX.push_back(ms.X.rel);
                //}

                //if(!mMouseSmoothY.empty()&&Ogre::Math::Abs(ms.Y.rel-mMouseSmoothY[mMouseSmoothY.size()-1])>100.f)
                //{
                //    mMouseSmoothY.push_back(ms.Y.rel-mMouseSmoothY[mMouseSmoothY.size()-1]);
                //}
                //else
                //{
                    mMouseSmoothY.push_back(ms.Y.rel);
                //}

				//mMouseSmoothY.push_back(ms.Y.rel);
				//std::cout<<"Mouse: "<<ms.X.abs<<" "<<ms.Y.abs<<"\n";//<<ms.X.rel<<" "<<ms.Y.rel<<" "
				mMouseSmoothTime.push_back(static_cast<float>(clock())/1000.f);

				float xTotal = 0.0f;
				float yTotal = 0.0f;

				for(int i = 0;i<mMouseSmoothTime.size();++i)
				{
					if((static_cast<float>(clock())/1000.f-mMouseSmoothTime[i])>0.075f)
					{
						mMouseSmoothTime.erase(mMouseSmoothTime.begin()+i);
						mMouseSmoothX.erase(mMouseSmoothX.begin()+i);
						mMouseSmoothY.erase(mMouseSmoothY.begin()+i);
					}
					else
					{
						xTotal+=static_cast<float>(mMouseSmoothX[i]);
						yTotal+=static_cast<float>(mMouseSmoothY[i]);
					}
				}

				xTotal/=mMouseSmoothX.size();
				yTotal/=mMouseSmoothY.size();

				mCameraYawNode->yaw(Ogre::Radian(Ogre::Degree((xTotal)*(-3.25f)*sensitivity)));//-TimeManager::getSingletonPtr()->getDeltaTime()

				float pitchChange = Ogre::Degree((yTotal)*(-3.25f)*sensitivity).valueDegrees();//*-TimeManager::getSingletonPtr()->getDeltaTime() a

				// limit the pitch
				if(mCurrentPitch+pitchChange>=mPitchLimitUp)
				{
					pitchChange = mPitchLimitUp-mCurrentPitch;
				}
				else if(mCurrentPitch+pitchChange<=-mPitchLimitDown)
				{
					pitchChange = -mPitchLimitDown-mCurrentPitch;
				}

				mCameraPitchNode->pitch(Ogre::Radian(Ogre::Degree(pitchChange)));
				mCurrentPitch+=pitchChange;
		}

		Ogre::WindowEventUtilities::messagePump();
		mRoot->renderOneFrame();
	}

	void GraphicsManager::setCameraRot(bool enabled)
	{
		if(camLook==enabled)
			return;

		camLook = enabled;

		if(!camLook)
		{
			//reset all of these
			mCameraPosNode->setPosition(Ogre::Vector3(0,0,0));
			mCameraNode->setPosition(Ogre::Vector3(0,0,0));
			mCameraRollNode->setPosition(Ogre::Vector3(0,0,0));
			mCameraPitchNode->setPosition(Ogre::Vector3(0,0,0));
			mCameraYawNode->setPosition(Ogre::Vector3(0,0,0));

			mCameraPitchNode->setOrientation(Ogre::Quaternion::IDENTITY);
			mCameraPosNode->setOrientation(Ogre::Quaternion::IDENTITY);
			mCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
			mCameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);
			mCameraRollNode->setOrientation(Ogre::Quaternion::IDENTITY);

			mCurrentPitch = 0.f;
		}
	}
}