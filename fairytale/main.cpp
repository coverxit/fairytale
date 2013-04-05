/*
 * Copyright 2012-2013 TennenColl
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pch.h"

#include <boost/thread.hpp>

#include "engine/engine.h"
#include "engine/input.h"
#include "engine/graphics.h"

using namespace std;
using namespace fairytale;
using namespace chaiscript;
using namespace Ogre;

boost::scoped_ptr<engine::Engine> gameEngine(new engine::Engine);
boost::scoped_ptr<boost::thread> inputThread;

void waitForUserInput()
{
	std::string cmd;
	std::cout << "Console is now available!\n>>> ";
	while(true)
	{
		std::getline(std::cin, cmd);
		gameEngine->appendScriptCommand(cmd);
		std::cin.clear();
		fflush(stdin);
	}
}

void registerScript(chaiscript::ChaiScript* script)
{
	(*script)
		.add(fun(boost::function<void()>(boost::bind(&engine::Engine::stop, gameEngine.get()))), "exit")
		.add(fun(boost::function<void()>(boost::bind(&engine::GraphicManager::takeScreenshot, gameEngine->getGraphicManager()))), "takeScreenshot")
		;
}


//-------------------------------------------------------------------------

#include "game/debug-cameraman.h"

namespace fairytale
{
	class GameScene : public Ogre::WindowEventListener
	{
	public:
		Ogre::SceneManager*									defaultSceneMgr;
		Ogre::Camera*										defaultCamera;
		boost::shared_ptr<DebugCameraMan>					debugCameraMan;
		boost::scoped_ptr<DebugCameraManFrameListener>		debugCameraManFrameListener;

		GameScene();

		void windowResized(Ogre::RenderWindow* rw);
	};
}

namespace fairytale
{
	GameScene::GameScene()
	{
		defaultSceneMgr = gameEngine->getGraphicManager()->getOgreRoot()->createSceneManager(Ogre::ST_GENERIC, "_default_scenemgr");
		defaultSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));
		defaultCamera = defaultSceneMgr->createCamera("PlayerCam");
		defaultCamera->setFarClipDistance(10000);
		defaultCamera->setNearClipDistance(0.05);
		defaultCamera->setPosition(Vector3(10,10,10));
		defaultCamera->lookAt(Vector3::ZERO);
		defaultCamera->setAspectRatio(gameEngine->getGraphicManager()->getViewportAspectRatio());

		gameEngine->getGraphicManager()->setCamera(defaultCamera);

		debugCameraMan.reset(new DebugCameraMan(defaultCamera));
		debugCameraMan->setTopSpeed(100);

		gameEngine->getInputManager()->registerKeyListener(debugCameraMan);
		gameEngine->getInputManager()->registerMouseListener(debugCameraMan);

		debugCameraManFrameListener.reset(new DebugCameraManFrameListener(debugCameraMan));
		gameEngine->getGraphicManager()->addFrameListener(debugCameraManFrameListener.get());
		gameEngine->getGraphicManager()->addWindowEventListener(this);
	}

	void GameScene::windowResized(Ogre::RenderWindow* rw)
	{
		defaultCamera->setAspectRatio(gameEngine->getGraphicManager()->getViewportAspectRatio());
	}
}

#include <Volume/OgreVolumeChunk.h>
#include <Volume/OgreVolumeCSGSource.h>
#include <Volume/OgreVolumeCacheSource.h>
#include <Volume/OgreVolumeTextureSource.h>
#include <Volume/OgreVolumeMeshBuilder.h>
#include <Volume/OgreVolumeSimplexNoise.h>

using namespace Ogre;
using namespace Ogre::Volume;

namespace fairytale
{
	class VolumeTerrainTest
	{
	public:
		boost::shared_ptr<Chunk> mVolumeRoot;
		boost::shared_ptr<GameScene> scene;
		VolumeTerrainTest()
		{
			scene.reset(new GameScene());

			// Skydome
			scene->defaultSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

			// Light
			Light* directionalLight0 = scene->defaultSceneMgr->createLight("directionalLight0");
			directionalLight0->setType(Light::LT_DIRECTIONAL);
			directionalLight0->setDirection(Vector3((Real)1, (Real)-1, (Real)1));
			directionalLight0->setDiffuseColour((Real)1, (Real)0.98, (Real)0.73);
			directionalLight0->setSpecularColour((Real)0.1, (Real)0.1, (Real)0.1);

			// Volume
			mVolumeRoot.reset(new Chunk());
			SceneNode *volumeRootNode = scene->defaultSceneMgr->getRootSceneNode()->createChildSceneNode("VolumeParent");
			mVolumeRoot->load(volumeRootNode, scene->defaultSceneMgr, "volumeTerrain.cfg");

			scene->defaultCamera->setPosition((Real)(2560 - 384), (Real)2000, (Real)(2560 - 384));
			scene->defaultCamera->lookAt((Real)0, (Real)100, (Real)0);
		}
	};
}

boost::scoped_ptr<VolumeTerrainTest> vtt;

int main()
{
	registerScript(gameEngine->getScript());

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_SYSRQ, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::GraphicManager::takeScreenshot, gameEngine->getGraphicManager())));

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_ESCAPE, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::Engine::stop, gameEngine.get())));

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_F10, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::Engine::appendGraphicManipulation, gameEngine.get(),
		[]() { vtt.reset(new VolumeTerrainTest); })));

	try
	{
		gameEngine->getGraphicManager()->addResourceLocation("resources");
		gameEngine->start();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{
	}

	system("pause");

	return 0;
}
