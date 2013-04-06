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
#include "engine/physics.h"

#include "btogre-fork/BtOgreExtras.h"
#include "btogre-fork/BtOgreGP.h"
#include "btogre-fork/BtOgrePG.h"

using namespace fairytale;
using namespace chaiscript;
using namespace Ogre;

boost::scoped_ptr<engine::Engine> gameEngine(new engine::Engine);

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

class BtOgreTestApplication : public GameScene
{
protected:


	Ogre::SceneNode *mNinjaNode;
	Ogre::Entity *mNinjaEntity;
	btRigidBody *mNinjaBody;
	btCollisionShape *mNinjaShape;

	Ogre::Entity *mGroundEntity;
	btRigidBody *mGroundBody;
	btBvhTriangleMeshShape *mGroundShape;

public:
	~BtOgreTestApplication()
	{
		//Free rigid bodies
		gameEngine->getPhysicsWorld()->removeRigidBody(mNinjaBody);
		delete mNinjaBody->getMotionState();
		delete mNinjaBody;
		delete mNinjaShape;

		gameEngine->getPhysicsWorld()->removeRigidBody(mGroundBody);
		delete mGroundBody->getMotionState();
		delete mGroundBody;
		delete mGroundShape->getMeshInterface();
		delete mGroundShape;
	}

	BtOgreTestApplication()
	{
		//----------------------------------------------------------
		// Ninja!
		//----------------------------------------------------------

		Vector3 pos = Vector3(0,10,0);
		Quaternion rot = Quaternion::IDENTITY;

		//Create Ogre stuff.

		mNinjaEntity = defaultSceneMgr->createEntity("ninjaEntity", "Player.mesh");
		mNinjaNode = defaultSceneMgr->getRootSceneNode()->createChildSceneNode("ninjaSceneNode", pos, rot);
		mNinjaNode->attachObject(mNinjaEntity);

		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(mNinjaEntity);
		mNinjaShape = converter.createSphere();

		//Calculate inertia.
		btScalar mass = 5;
		btVector3 inertia;
		mNinjaShape->calculateLocalInertia(mass, inertia);

		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *ninjaState = new BtOgre::RigidBodyState(gameEngine.get(), mNinjaNode);

		//Create the Body.
		mNinjaBody = new btRigidBody(mass, ninjaState, mNinjaShape, inertia);
		gameEngine->getPhysicsWorld()->addRigidBody(mNinjaBody);

		//----------------------------------------------------------
		// Ground!
		//----------------------------------------------------------

		//Create Ogre stuff.
		//MeshManager::getSingleton().createPlane("groundPlane", "General", Plane(Vector3::UNIT_Y, 0), 100, 100, 
		//10, 10, true, 1, 5, 5, Vector3::UNIT_Z);
		mGroundEntity = defaultSceneMgr->createEntity("groundEntity", "TestLevel_b0.mesh");
		//mGroundEntity->setMaterialName("Examples/Rockwall");
		defaultSceneMgr->getRootSceneNode()->createChildSceneNode("groundNode")->attachObject(mGroundEntity);

		//Create the ground shape.
		BtOgre::StaticMeshToShapeConverter converter2(mGroundEntity);
		mGroundShape = converter2.createTrimesh();

		//Create MotionState (no need for BtOgre here, you can use it if you want to though).
		btDefaultMotionState* groundState = new btDefaultMotionState(
			btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));

		//Create the Body.
		mGroundBody = new btRigidBody(0, groundState, mGroundShape, btVector3(0,0,0));
		gameEngine->getPhysicsWorld()->addRigidBody(mGroundBody);
	}
};

boost::scoped_ptr<BtOgreTestApplication> vtt;

int main()
{
	registerScript(gameEngine->getScript());

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_SYSRQ, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::GraphicManager::takeScreenshot, gameEngine->getGraphicManager())));

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_ESCAPE, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::Engine::stop, gameEngine.get())));

	gameEngine->getInputManager()->registerKeyBinding(OIS::KC_F10, engine::InputManager::KeyState::DOWN,
		boost::function<void(const OIS::KeyEvent&)>(boost::bind(&engine::Engine::appendGraphicManipulation, gameEngine.get(),
		[]() { vtt.reset(new BtOgreTestApplication); })));

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
