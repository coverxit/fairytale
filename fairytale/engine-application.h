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
#include <SkyX.h>
#include <Ogre/SdkCameraMan.h>

struct SkyXSettings;

namespace fairytale
{
	class CoreMembers : public fairytale::Singleton<CoreMembers>
	{
	public:
		boost::scoped_ptr<Ogre::Root>			ogreRoot;
		Ogre::RenderWindow*						renderWnd;
		Ogre::Viewport*							defaultViewport;
		Ogre::Log*								defaultLog;
		Ogre::Timer*							mainLoopTimer;

		OIS::InputManager*						inputMgr;
		OIS::Keyboard*							keyboard;
		OIS::Mouse*								mouse;

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
			mainLoopTimer	= 0;

			inputMgr		= 0;
			keyboard		= 0;
			mouse			= 0;

			_inited			= false;
			_shutdown		= false;
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
