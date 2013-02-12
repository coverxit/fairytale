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

#ifndef __FAIRYTALE_ENGINE_APPLICATION_H__
#define __FAIRYTALE_ENGINE_APPLICATION_H__

#include "engine-pch.h"
#include "game-env.h"
#include "util-singleton.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

namespace fairytale
{
	class CoreMembers : public fairytale::Singleton<CoreMembers>
	{
	public:
		boost::scoped_ptr<Ogre::Root>			ogreRoot;
		Ogre::RenderWindow*						renderWnd;
		Ogre::Viewport*							defaultViewport;
		Ogre::Log*								defaultLog;
		Ogre::Camera*							defaultCam;
		Ogre::SceneManager*						defaultSceneMgr;

		OIS::InputManager*						inputMgr;
		OIS::Keyboard*							keyboard;
		OIS::Mouse*								mouse;

		boost::scoped_ptr<btAxisSweep3>							broadPhase;
		boost::scoped_ptr<btDefaultCollisionConfiguration>		collisionConfig;
		boost::scoped_ptr<btCollisionDispatcher>				dispatcher;
		boost::scoped_ptr<btSequentialImpulseConstraintSolver>	solver;
		boost::scoped_ptr<btDynamicsWorld>						phyWorld;
		boost::scoped_ptr<BtOgre::DebugDrawer>					dbgDraw;

		friend class Application;

	private:
		ThreadPtr								_gameConsole;
		std::deque<std::string>					_commands;
		boost::mutex							_commandsDequeMutex;
		bool									_inited;
		bool									_shutdown;

	public:
		CoreMembers()
		{
			renderWnd		= 0;
			defaultViewport	= 0;
			defaultLog		= 0;
			defaultCam		= 0;
			defaultSceneMgr	= 0;

			inputMgr		= 0;
			keyboard		= 0;
			mouse			= 0;

			_inited			= false;
			_shutdown		= false;
		}

		~CoreMembers()
		{
			dbgDraw.reset();
			phyWorld.reset();

			solver.reset();
			dispatcher.reset();
			collisionConfig.reset();
			broadPhase.reset();

			ogreRoot.reset();
		}
	};

	class Application
	{
	public:
		void initOgre(const Ogre::String& logFile, const Ogre::String& configFile);
		bool showConfigDialog();
		void initWindow(const Ogre::String& wndtitle);
		void startLoop();
		void shutdown();

		void addResourceLocation(const std::string& dir);
		void loadPlugin(const Ogre::String& filename);
		void loadPluginsFromDirectory(const std::string& dir);

	private:
		void _doMainLoop();
		void _waitForUserInput();
	};
};

#endif
