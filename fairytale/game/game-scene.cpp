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

#include "game-scene.h"

#include "engine/graphics.h"
#include "engine/input.h"

using namespace Ogre;

namespace fairytale
{
	GameScene::GameScene()
	{
		using namespace engine::graphics;

		defaultSceneMgr = getOGRE()->createSceneManager(Ogre::ST_GENERIC, "_default_scenemgr");

		defaultSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));

		defaultCamera = defaultSceneMgr->createCamera("PlayerCam");

		defaultCamera->setFarClipDistance(10000);
		defaultCamera->setNearClipDistance(0.05);
		defaultCamera->setPosition(Vector3(10,10,10));
		defaultCamera->lookAt(Vector3::ZERO);
		defaultCamera->setAspectRatio(Real(getDefaultViewport()->getActualWidth()) / Real(getDefaultViewport()->getActualHeight()));

		getDefaultViewport()->setCamera(defaultCamera);

		debugCameraMan.reset(new DebugCameraMan(defaultCamera));
		debugCameraMan->setTopSpeed(100);

		{
			using namespace engine::input;
			registerKeyListener("SDK_CAMERA_MAN", debugCameraMan);
			registerMouseListener("SDK_CAMERA_MAN", debugCameraMan);
		}

		debugCameraManFrameListener.reset(new DebugCameraManFrameListener(debugCameraMan));
		getOGRE()->addFrameListener(debugCameraManFrameListener.get());
		Ogre::WindowEventUtilities::addWindowEventListener(getDefaultRenderWindow(), this);
	}

	void GameScene::windowResized(Ogre::RenderWindow* rw)
	{
		using namespace engine::graphics;
		engine::input::setMouseBufferedArea(getDefaultRenderWindow()->getWidth(), getDefaultRenderWindow()->getHeight());
		defaultCamera->setAspectRatio(Real(getDefaultViewport()->getActualWidth()) / Real(getDefaultViewport()->getActualHeight()));
	}
}
