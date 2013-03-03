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

#ifndef __FAIRYTALE_GAME_TERRAIN_H__
#define __FAIRYTALE_GAME_TERRAIN_H__

#include "pch.h"

#include <Volume/OgreVolumeChunk.h>
#include <Volume/OgreVolumeCSGSource.h>
#include <Volume/OgreVolumeCacheSource.h>
#include <Volume/OgreVolumeTextureSource.h>
#include <Volume/OgreVolumeMeshBuilder.h>
#include <Volume/OgreVolumeSimplexNoise.h>

#include "game-scene.h"
#include "debug-cameraman.h"

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

#endif
