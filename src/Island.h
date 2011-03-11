#pragma once
#include "stdafx.h"

namespace LD
{
	class GameManager;
	class GfxObj;
	class PhysicsObject;

	class Island
	{
	public:

		Island(GameManager* gmgr,std::string mesh,Ogre::Vector3 pos,float speedval,unsigned int type);
		~Island();

		void landedOn();

		void update(float delta);

		void setVisible(bool visible);

		void reactivate(Ogre::Vector3 pos);

		void anomalize();

		GfxObj* object;
		GfxObj* pickup;
		Ogre::MaterialPtr materialCopy;
		bool readyForDelete;
		GameManager* gmg;
		PhysicsObject* pobj;

		bool hathBeenTroddenUpon;

		unsigned int tp;


	};

	struct IslandArrangement
	{
		IslandArrangement(){}
		~IslandArrangement(){}

		void addMesh(std::string name,Ogre::Vector3 pos)
		{
			positions.push_back(pos);
			meshes.push_back(name);
		}

		std::vector<std::string> meshes;
		std::vector<Ogre::Vector3> positions;
		float difficulty;
		unsigned int nrMeshes;
	};
}