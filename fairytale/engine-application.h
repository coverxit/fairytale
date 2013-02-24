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

#ifndef __FAIRYTALE_ENGINE_APPLICATION_H__
#define __FAIRYTALE_ENGINE_APPLICATION_H__

#include "engine-pch.h"
#include "util-singleton.h"

namespace fairytale
{
	class Application : public Singleton<Application>
	{
	public:
		Application();
		~Application();

		void startLoop();
		void shutdown();

		void addResourceLocation(const std::string& dir);
		void loadPlugin(const std::string& filename);
		void loadPluginsFromDirectory(const std::string& dir);

		void takeScreenshot();

	private:
		void _doMainLoop();
		void _waitForUserInput();

	public:
		Ogre::Root* const				ogreRoot;
		Ogre::RenderWindow* const		renderWnd;
		Ogre::Viewport* const			defaultViewport;

		OIS::InputManager* const		inputMgr;
		OIS::Keyboard* const			keyboard;
		OIS::Mouse* const				mouse;

	private:
		ThreadPtr						_gameConsole;
		std::deque<std::string>			_commands;
		boost::mutex					_commandsDequeMutex;
		bool							_shutdown;
	};
};

#endif
