#include "stdafx.h"
#include "PhysicsManager.h"

#include "BulletCollision/CollisionShapes/btShapeHull.h"

namespace LD
{
	PhysicsManager::PhysicsManager()
		:started(0),mDynamicsWorld(0),mAccumulation(0),mInterpolation(0),mTimeStep(1.f/100.f)
	{}

	PhysicsManager::~PhysicsManager()
	{
		deinit();
	}

	void PhysicsManager::init()
	{
		mDynamicsWorld = NULL;

		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher = new	btCollisionDispatcher(mCollisionConfiguration);
		mBroadphase = new btDbvtBroadphase();
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		mSolver = sol;
		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher,mBroadphase,mSolver,mCollisionConfiguration);

		mDynamicsWorld->setGravity(btVector3(0,-10.f,0));
		started = true;

		mAccumulation = 0;
		mInterpolation = 0;
		mTimeStep = 1.0f/100.0f;
	}

	void PhysicsManager::deinit()
	{
		if(started)
		{
			
			// delete this firtst since bullet asserts if you delete a body w/ constraints
			for(unsigned int j=0;j<mControllers.size();++j)
			{
				delete mControllers[j];
			}
			mControllers.clear();

			std::cout<<"\n\n\nReleasing Bullet....\n\n\n";
			int i;
			for (i=mDynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
			{
				btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body)
				{
					delete body->getMotionState();
				}
				mDynamicsWorld->removeCollisionObject( obj );
				delete obj;
			}

			for(unsigned int j=mObjects.size();j<mObjects.size();++j)
			{
				delete mObjects[j];
			}
			mObjects.clear();

			// delete all this wonderful stuff...
			delete mDynamicsWorld;
			delete mSolver;
			delete mBroadphase;
			delete mDispatcher;
			delete mCollisionConfiguration;

			started = false;
		}
	}

	void PhysicsManager::update(float delta)
	{
		if(started)
		{
			mAccumulation += delta;

			while(mAccumulation>=mTimeStep)
			{
				//for(unsigned int i=0;i<mControllers.size();++i)
				//{
				//	mControllers[i]->update(delta,false,0);
				//}

				for(unsigned int i=0;i<mControllers.size();++i)
				{
					mControllers[i]->setVeloc();//->update(delta,true,0);
				}

				mDynamicsWorld->stepSimulation(mTimeStep,0);

				for(unsigned int i=0;i<mObjects.size();++i)
				{
					mObjects[i]->update(0,true);
				}
				for(unsigned int i=0;i<mControllers.size();++i)
				{
					mControllers[i]->update(delta,true,0);
				}

				mAccumulation -= mTimeStep;
			}

			mInterpolation = mAccumulation/mTimeStep;
			for(unsigned int i=0;i<mObjects.size();++i)
			{
				if(!mObjects[i]->deleteMe)
				{
					mObjects[i]->update(mInterpolation,false);
				}
				else
				{
					mDynamicsWorld->removeCollisionObject(mObjects[i]->mObject);
					delete mObjects[i];
					mObjects.erase(mObjects.begin()+i);
					--i;
				}
			}
			for(unsigned int i=0;i<mControllers.size();++i)
			{
				mControllers[i]->update(delta,false,mInterpolation);
			}
		}
	}

	void PhysicsManager::clearCache()
	{
		//delete cached collision shapes
		std::map<std::string,btCollisionShape*>::iterator itera = mShapes.begin();
		while(itera!=mShapes.end())
		{
			delete itera->second;
			itera->second = 0;
			++itera;
		}
		mShapes.clear();
	}

	PhysicsObject* PhysicsManager::createStaticTrimesh(std::string meshname,Ogre::Vector3 pos,float _restitution,float _friction)
	{
		btCollisionShape *triMeshShape = 0;
		btVector3* vertices;
		unsigned* indices;
		bool allocatedData = false;

		if(mShapes.find(meshname)!=mShapes.end())
		{
			triMeshShape = mShapes[meshname];
		}
		else
		{
			allocatedData = true;
			size_t vertex_count;
			float vArray[9];
			size_t numTris;
			size_t index_count;
			vertex_count = index_count = 0;
			bool added_shared = false;
			size_t current_offset = vertex_count;
			size_t shared_offset = vertex_count;
			size_t next_offset = vertex_count;
			size_t index_offset = index_count;
			size_t prev_vert = vertex_count;
			size_t prev_ind = index_count;


			btTriangleMesh *mTriMesh = new btTriangleMesh();

			Ogre::MeshPtr mesh = Ogre::MeshManager::getSingletonPtr()->load(meshname,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);//Ogre::MeshPtr(mesh_);

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				if(submesh->useSharedVertices)
				{
					if(!added_shared)
					{
						Ogre::VertexData* vertex_data = mesh->sharedVertexData;
						vertex_count += vertex_data->vertexCount;
						added_shared = true;
					}
				}
				else
				{
					Ogre::VertexData* vertex_data = submesh->vertexData;
					vertex_count += vertex_data->vertexCount;
				}

				Ogre::IndexData* index_data = submesh->indexData;
				index_count += index_data->indexCount;
			}

			int a = vertex_count;
			vertices = new btVector3[vertex_count];
			indices = new unsigned[index_count];

			added_shared = false;

			int rVT = 0;
			int rIT = 0;

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
				if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
				{
					if(submesh->useSharedVertices)
					{
						added_shared = true;
						shared_offset = current_offset;
					}

					const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
					Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
					unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
					Ogre::Real* pReal;

					for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
					{
						posElem->baseVertexPointerToElement(vertex, &pReal);

						Ogre::Vector3 pt;

						pt.x = (*pReal++);
						pt.y = (*pReal++);
						pt.z = (*pReal++);

						vertices[rVT + current_offset + j].setX(pt.x);
						vertices[rVT + current_offset + j].setY(pt.y);
						vertices[rVT + current_offset + j].setZ(pt.z);
					}

					vbuf->unlock();
					next_offset += vertex_data->vertexCount;
				}

				Ogre::IndexData* index_data = submesh->indexData;

				numTris = index_data->indexCount / 3;
				unsigned short* pShort;
				unsigned int* pInt;
				Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
				bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
				if (use32bitindexes) pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
				else pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

				for(size_t k = 0; k < numTris; ++k)
				{
					size_t offset = (submesh->useSharedVertices)?shared_offset:current_offset;

					unsigned int vindex = use32bitindexes? *pInt++ : *pShort++;
					indices[rIT + index_offset + 0] = vindex + offset;
					vArray[0] =  vertices[indices[index_offset + 0]].x();
					vArray[1] =  vertices[indices[index_offset + 0]].y();
					vArray[2] =  vertices[indices[index_offset + 0]].z();
					vindex = use32bitindexes? *pInt++ : *pShort++;
					indices[rIT + index_offset + 1] = vindex + offset;
					vArray[3] =  vertices[indices[index_offset + 1]].x();
					vArray[4] =  vertices[indices[index_offset + 1]].y();
					vArray[5] =  vertices[indices[index_offset + 1]].z();
					vindex = use32bitindexes? *pInt++ : *pShort++;
					indices[rIT + index_offset + 2] = vindex + offset;
					vArray[6] =  vertices[indices[index_offset + 2]].x();
					vArray[7] =  vertices[indices[index_offset + 2]].y();
					vArray[8] =  vertices[indices[index_offset + 2]].z();

					mTriMesh->addTriangle(btVector3(vArray[0],vArray[1],vArray[2]),
						btVector3(vArray[3],vArray[4],vArray[5]),
						btVector3(vArray[6],vArray[7],vArray[8]));

					index_offset += 3;
				}
				ibuf->unlock();
				current_offset = next_offset;
			}

			triMeshShape = new btBvhTriangleMeshShape(mTriMesh,true);
			mShapes[meshname] = triMeshShape;
		}

		btVector3 localInertia(0,0,0);
		btCollisionObject* actor = new btCollisionObject();
		actor->setCollisionShape(triMeshShape);
		actor->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(pos.x,pos.y,pos.z)));
		actor->setRestitution(_restitution);
		actor->setFriction(_friction);
		mDynamicsWorld->addCollisionObject(actor,LD::COL_STATICS);

		mObjects.push_back(new PhysicsObject(actor));

		if(allocatedData)
		{
			delete[] vertices;
			delete[] indices;
		}

		PhysicsObject* ob = mObjects[mObjects.size()-1];
		return ob;
	}

	PhysicsObject* PhysicsManager::createConvexHull(std::string meshname,Ogre::Vector3 pos,Ogre::Vector3 scale,float _mass,float _restitution,float _friction)
	{
		if(meshname=="CUBE"||meshname=="BOX")
		{
			return createCube(scale,pos,_mass,_restitution,_friction);
		}

		btConvexShape *convexShape;
		btShapeHull* hull;
		btConvexShape *finalShape = 0;
		float* vertices;
		std::vector<Ogre::Vector3> vertVect;
		bool allocatedData = false;

		if(mShapes.find(meshname)!=mShapes.end())
		{
			finalShape = dynamic_cast<btConvexShape*>(mShapes[meshname]);
		}
		else
		{
			allocatedData = true;
			size_t vertex_count;
			size_t index_count;
			vertex_count = index_count = 0;

			bool added_shared = false;
			size_t current_offset = vertex_count;
			size_t shared_offset = vertex_count;
			size_t next_offset = vertex_count;
			size_t index_offset = index_count;
			size_t prev_vert = vertex_count;
			size_t prev_ind = index_count;

			Ogre::MeshPtr mesh = Ogre::MeshManager::getSingletonPtr()->load(meshname,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				if(submesh->useSharedVertices)
				{
					if(!added_shared)
					{
						Ogre::VertexData* vertex_data = mesh->sharedVertexData;
						vertex_count += vertex_data->vertexCount;
						added_shared = true;
					}
				}
				else
				{
					Ogre::VertexData* vertex_data = submesh->vertexData;
					vertex_count += vertex_data->vertexCount;
				}

				Ogre::IndexData* index_data = submesh->indexData;
				index_count += index_data->indexCount;
			}

			int a = vertex_count;

			vertices = new float[vertex_count*3];

			added_shared = false;

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
				if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
				{
					if(submesh->useSharedVertices)
					{
						added_shared = true;
						shared_offset = current_offset;
					}

					const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
					Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
					unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
					Ogre::Real* pReal;

					for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
					{
						posElem->baseVertexPointerToElement(vertex, &pReal);

						Ogre::Vector3 pt;

						pt.x = (*pReal++);
						pt.y = (*pReal++);
						pt.z = (*pReal++);

						bool skip = false;

						// ignore duplicates
						for(unsigned int i=0;i<vertVect.size();++i)
						{
							if(pt.squaredDistance(vertVect[i])==0||vertVect[i]==pt)
							{
								skip = true;
							}
						}

						if(!skip)
						{
							vertices[current_offset + (j*3)] = pt.x*0.92f;
							vertices[current_offset + (j*3) + 1] = pt.y*0.92f;
							vertices[current_offset + (j*3) + 2] = pt.z*0.92f;
							vertVect.push_back(pt);
						}
					}
					vbuf->unlock();
					next_offset += vertex_data->vertexCount;
				}
			}


			convexShape = new btConvexHullShape(static_cast<btScalar*>(vertices),vertVect.size(),3*sizeof(btScalar));

			// if it's a complex enough mesh then run it through the simplifer thing...
			if(vertVect.size()>75)
			{
				hull = new btShapeHull(convexShape);
				btScalar margin = convexShape->getMargin();
				hull->buildHull(margin);
				
				btConvexHullShape* simplifiedConvexShape = new btConvexHullShape();
				for (int i=0;i<hull->numVertices();i++)
				{
					btVector3 vect = hull->getVertexPointer()[i]*0.9f;
					simplifiedConvexShape->addPoint(vect);
				}

				mShapes[meshname] = simplifiedConvexShape;
				finalShape = simplifiedConvexShape;
			}
			else
			{
				mShapes[meshname] = convexShape;
				finalShape = convexShape;
			}

		}

		btVector3 localInertia(0,0,0);
		finalShape->calculateLocalInertia(_mass,localInertia);

		btRigidBody* actor = new btRigidBody(_mass,0,finalShape,localInertia);	
		actor->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(pos.x,pos.y,pos.z)));
		actor->setRestitution(_restitution);
		actor->setFriction(_friction);
		//actor->setDamping(0.f,0.5f); bullet seems to apply a lot of torque to pretty much everything (compared to PhysX)... so maybe try some damping?
		//mDynamicsWorld->addRigidBody(actor);
		dynamic_cast<btDiscreteDynamicsWorld*>(mDynamicsWorld)->addRigidBody(actor,COL_DYNAMICS,COL_DYNAMICS|COL_KINEMATICS|COL_STATICS); //if I need collision filtering..

		mObjects.push_back(new PhysicsObject(actor));

		if(allocatedData)
		{
			delete[] vertices;
		}
		if(vertVect.size()>75)
		{
			delete hull;
			delete convexShape;
		}

		return mObjects[mObjects.size()-1];
	}

	PhysicsObject* PhysicsManager::createCube(Ogre::Vector3 scale,Ogre::Vector3 pos,float _mass,float _restitution,float _friction)
	{
		btBoxShape *boxShape;
		// I dunno if bullet makes a copy of the shape, so I'll only use a cached copy when the dimensions are the same?..
		if(mShapes.find("Box"+Ogre::StringConverter::toString(scale))==mShapes.end())
		{
			boxShape = new btBoxShape(Vector3_bt(scale));
			mShapes["Box"+Ogre::StringConverter::toString(scale)] = boxShape;
		}
		else
		{
			boxShape = dynamic_cast<btBoxShape*>(mShapes["Box"+Ogre::StringConverter::toString(scale)]);
		}

		btVector3 localInertia(0,0,0);
		boxShape->calculateLocalInertia(_mass,localInertia);

		btRigidBody* actor = new btRigidBody(_mass,0,boxShape,localInertia);	
		actor->setRestitution(_restitution);
		actor->setFriction(_friction);
		actor->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(pos.x,pos.y,pos.z)));

		//mDynamicsWorld->addRigidBody(actor);
		dynamic_cast<btDiscreteDynamicsWorld*>(mDynamicsWorld)->addRigidBody(actor,COL_DYNAMICS,COL_DYNAMICS|COL_KINEMATICS|COL_STATICS); //if I need collision filtering..

		//dynamic_cast<btDiscreteDynamicsWorld*>(mDynamicsWorld)->addRigidBody(actor,COL_DYNAMICS,COL_DYNAMICS|COL_KINEMATICS|COL_STATICS); if I need collision filtering..
		mObjects.push_back(new PhysicsObject(actor));

		return mObjects[mObjects.size()-1];
	}


	CharacterController* PhysicsManager::createCCTCap(Ogre::Vector3 scale,Ogre::Vector3 pos,float _mass,float _restitution,float _friction)
	{
		btCapsuleShape *capsule;
		// I dunno if bullet makes a copy of the shape, so I'll only use a cached copy when the dimensions are the same?..
		if(mShapes.find("caps"+Ogre::StringConverter::toString(scale))==mShapes.end())
		{
			capsule = new btCapsuleShape(0.5f,1.f);
			mShapes["caps"+Ogre::StringConverter::toString(scale)] = capsule;
		}
		else
		{
			capsule = dynamic_cast<btCapsuleShape*>(mShapes["caps"+Ogre::StringConverter::toString(scale)]);
		}

		btVector3 localInertia(0,0,0);
		capsule->calculateLocalInertia(_mass,localInertia);

		btRigidBody* actor = new btRigidBody(_mass,0,capsule,localInertia);	
		actor->setRestitution(_restitution);
		actor->setFriction(_friction);
		actor->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(pos.x,pos.y,pos.z)));

		//mDynamicsWorld->addRigidBody(actor);
		dynamic_cast<btDiscreteDynamicsWorld*>(mDynamicsWorld)->addRigidBody(actor,COL_KINEMATICS,COL_DYNAMICS|COL_KINEMATICS|COL_STATICS); //if I need collision filtering..
		mControllers.push_back(new CharacterController(this,actor,pos));

		return mControllers[mControllers.size()-1];
	}


	CharacterController* PhysicsManager::createCharacterController(std::string meshname,Ogre::Vector3 pos,Ogre::Vector3 scale,float _mass,float _restitution,float _friction)
	{

		btConvexShape *convexShape;
		btShapeHull* hull;
		btConvexShape *finalShape = 0;
		float* vertices;
		std::vector<Ogre::Vector3> vertVect;
		bool allocatedData = false;

		if(mShapes.find(meshname)!=mShapes.end())
		{
			finalShape = dynamic_cast<btConvexShape*>(mShapes[meshname]);
		}
		else
		{
			allocatedData = true;
			size_t vertex_count;
			size_t index_count;
			vertex_count = index_count = 0;

			bool added_shared = false;
			size_t current_offset = vertex_count;
			size_t shared_offset = vertex_count;
			size_t next_offset = vertex_count;
			size_t index_offset = index_count;
			size_t prev_vert = vertex_count;
			size_t prev_ind = index_count;

			Ogre::MeshPtr mesh = Ogre::MeshManager::getSingletonPtr()->load(meshname,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				if(submesh->useSharedVertices)
				{
					if(!added_shared)
					{
						Ogre::VertexData* vertex_data = mesh->sharedVertexData;
						vertex_count += vertex_data->vertexCount;
						added_shared = true;
					}
				}
				else
				{
					Ogre::VertexData* vertex_data = submesh->vertexData;
					vertex_count += vertex_data->vertexCount;
				}

				Ogre::IndexData* index_data = submesh->indexData;
				index_count += index_data->indexCount;
			}

			int a = vertex_count;

			vertices = new float[vertex_count*3];

			added_shared = false;

			for(int i = 0;i < mesh->getNumSubMeshes();i++)
			{
				Ogre::SubMesh* submesh = mesh->getSubMesh(i);

				Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
				if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
				{
					if(submesh->useSharedVertices)
					{
						added_shared = true;
						shared_offset = current_offset;
					}

					const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
					Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
					unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
					Ogre::Real* pReal;

					for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
					{
						posElem->baseVertexPointerToElement(vertex, &pReal);

						Ogre::Vector3 pt;

						pt.x = (*pReal++);
						pt.y = (*pReal++);
						pt.z = (*pReal++);

						bool skip = false;

						// ignore duplicates
						for(unsigned int i=0;i<vertVect.size();++i)
						{
							if(pt.squaredDistance(vertVect[i])==0||vertVect[i]==pt)
							{
								skip = true;
							}
						}

						if(!skip)
						{
							vertices[current_offset + (j*3)] = pt.x*0.92f;
							vertices[current_offset + (j*3) + 1] = pt.y*0.92f;
							vertices[current_offset + (j*3) + 2] = pt.z*0.92f;
							vertVect.push_back(pt);
						}
					}
					vbuf->unlock();
					next_offset += vertex_data->vertexCount;
				}
			}


			convexShape = new btConvexHullShape(static_cast<btScalar*>(vertices),vertVect.size(),3*sizeof(btScalar));

			// if it's a complex enough mesh then run it through the simplifer thing...
			if(vertVect.size()>75)
			{
				hull = new btShapeHull(convexShape);
				btScalar margin = convexShape->getMargin();
				hull->buildHull(margin);
				
				btConvexHullShape* simplifiedConvexShape = new btConvexHullShape();
				for (int i=0;i<hull->numVertices();i++)
				{
					btVector3 vect = hull->getVertexPointer()[i]*0.9f;
					simplifiedConvexShape->addPoint(vect);
				}

				mShapes[meshname] = simplifiedConvexShape;
				finalShape = simplifiedConvexShape;
			}
			else
			{
				mShapes[meshname] = convexShape;
				finalShape = convexShape;
			}

		}

		finalShape->setMargin(0.2f);

		btVector3 localInertia(0,0,0);
		finalShape->calculateLocalInertia(_mass,localInertia);

		btRigidBody* actor = new btRigidBody(_mass,0,finalShape,localInertia);	
		actor->setWorldTransform(btTransform(btQuaternion::getIdentity(),btVector3(pos.x,pos.y,pos.z)));
		actor->setRestitution(_restitution);
		actor->setFriction(_friction);
		//actor->setDamping(0.f,0.5f); bullet seems to apply a lot of torque to pretty much everything (compared to PhysX)... so maybe try some damping?
		mDynamicsWorld->addRigidBody(actor);
		mControllers.push_back(new CharacterController(this,actor,pos));
		//mObjects.push_back(new PhysicsObject(actor));

		if(allocatedData)
		{
			delete[] vertices;
		}
		if(vertVect.size()>75)
		{
			delete hull;
			delete convexShape;
		}

		return mControllers[mControllers.size()-1];
	}
	
}