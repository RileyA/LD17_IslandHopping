#include "stdafx.h"
#include "GameManager.h"
#include "GfxObj.h"
#include "MenuState.h"
#include "PlayState.h"
#include "Utils.h"
#define NOCONSOLE

LD::GameManager* gmgr;

#ifdef NOCONSOLE
#include "windows.h"

LD::MenuState Menu_State;
LD::PlayState Play_State;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	std::cout<<"Ludum Dare Time!\n";
	gmgr = new LD::GameManager();
	gmgr->init();
	Menu_State = LD::MenuState(gmgr);
	//Play_State = LD::PlayState(gmgr);
	//LD::TestState state02 = LD::TestState(gmgr,Ogre::ColourValue(0,0,1));
	gmgr->addState(&Menu_State);
	//gmgr->addState(&Play_State);
	gmgr->go();
	gmgr->deinit();
	delete gmgr;
	gmgr = 0;
	//std::cin.get();
	return 0;
}
