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

#include "engine-pch.h"

#include "game-scene.h"
#include "engine-application.h"
#include "engine-input.h"

using namespace Ogre;

namespace fairytale
{
	GameScene::GameScene()
	{
		LOCK_AND_GET_INSTANCE_PTR(Application, app);

		defaultSceneMgr = app->ogreRoot->createSceneManager(Ogre::ST_GENERIC, "_default_scenemgr");

		defaultSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));

		defaultCamera = defaultSceneMgr->createCamera("PlayerCam");

		defaultCamera->setFarClipDistance(10000);
		defaultCamera->setNearClipDistance(0.05);
		defaultCamera->setPosition(Vector3(10,10,10));
		defaultCamera->lookAt(Vector3::ZERO);
		defaultCamera->setAspectRatio(Real(app->defaultViewport->getActualWidth()) / Real(app->defaultViewport->getActualHeight()));

		app->defaultViewport->setCamera(defaultCamera);

		debugCameraMan.reset(new DebugCameraMan(defaultCamera));
		debugCameraMan->setTopSpeed(100);

		{
			LOCK_AND_GET_INSTANCE_PTR(KeyListenerManager, Keyboard);
			Keyboard->registerListener("SDK_CAMERA_MAN", debugCameraMan);
			LOCK_AND_GET_INSTANCE_PTR(MouseListenerManager, Mouse);
			Mouse->registerListener("SDK_CAMERA_MAN", debugCameraMan);
		}

		debugCameraManFrameListener.reset(new DebugCameraManFrameListener(debugCameraMan));
		app->ogreRoot->addFrameListener(debugCameraManFrameListener.get());
		Ogre::WindowEventUtilities::addWindowEventListener(app->renderWnd, this);
	}

	void GameScene::windowResized(Ogre::RenderWindow* rw)
	{
		LOCK_AND_GET_INSTANCE_PTR(Application, app);
		boost::mutex::scoped_try_lock(_mutex);

		app->mouse->getMouseState().height = app->renderWnd->getHeight();
		app->mouse->getMouseState().width = app->renderWnd->getWidth();
		defaultCamera->setAspectRatio(Real(app->defaultViewport->getActualWidth()) / Real(app->defaultViewport->getActualHeight()));
	}
}
