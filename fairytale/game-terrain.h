/*
 * Copyright 2012 TennenColl
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

#include "engine-pch.h"

#include <Volume/OgreVolumeChunk.h>
#include <Volume/OgreVolumeCSGSource.h>
#include <Volume/OgreVolumeCacheSource.h>
#include <Volume/OgreVolumeTextureSource.h>
#include <Volume/OgreVolumeMeshBuilder.h>
#include <Volume/OgreVolumeSimplexNoise.h>

#include "engine-application.h"
#include "debug-cameraman.h"

using namespace Ogre;
using namespace Ogre::Volume;

extern boost::shared_ptr<fairytale::DebugCameraMan> debugCameraMan;
extern SceneManager* mSceneMgr;

namespace fairytale
{
	class VolumeTerrainTest
	{
	public:
		boost::shared_ptr<Chunk> mVolumeRoot;

		VolumeTerrainTest()
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

			// Skydome
			mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

			SimplexNoise sn(100);
			LogManager::getSingleton().stream() << sn.noise((Real)1.3, (Real)2.3, (Real)3.3);
			//LogManager::getSingleton().stream() << sn.noise(2, 3, 4);
			//LogManager::getSingleton().stream() << sn.noise(3, 4, 5);
			//LogManager::getSingleton().stream() << sn.noise(4, 5, 6);

			// Light
			Light* directionalLight0 = mSceneMgr->createLight("directionalLight0");
			directionalLight0->setType(Light::LT_DIRECTIONAL);
			directionalLight0->setDirection(Vector3((Real)1, (Real)-1, (Real)1));
			directionalLight0->setDiffuseColour((Real)1, (Real)0.98, (Real)0.73);
			directionalLight0->setSpecularColour((Real)0.1, (Real)0.1, (Real)0.1);

			// Volume
			mVolumeRoot.reset(new Chunk);
			SceneNode *volumeRootNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("VolumeParent");
			mVolumeRoot->load(volumeRootNode, mSceneMgr, "volumeTerrain.cfg");

		}
		~VolumeTerrainTest()
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);
			core->ogreRoot->destroySceneManager(mSceneMgr);
		}

	};
}

#endif
