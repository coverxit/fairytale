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
#include "util-singleton.h"
#include "game-env.h"
#include <SkyX.h>
#include <Ogre/SdkCameraMan.h>

struct SkyXSettings;

namespace fairytale
{
	class Application : public Ogre::FrameListener
	{
	public:
		void initOgre(const Ogre::String& logFile, const Ogre::String& configFile);
		void showConfigDialog();
		void initWindow(const Ogre::String& wndtitle);
		void startLoop();
		void shutdown();

		void addResourceLocation(const Ogre::String& group, const Ogre::String& type, const Ogre::String& archiveName);
		void addResourceLocationsFromFile(const Ogre::String& filename);
		void loadPlugin(const Ogre::String& filename);
		void loadPluginsFromDirectory(const std::string& dir);

		static boost::scoped_ptr<Ogre::Root>			ogreRoot;
		static Ogre::RenderWindow*						renderWnd;
		static Ogre::Viewport*							defaultViewport;
		static Ogre::Log*								defaultLog;
		static Ogre::Timer*								mainLoopTimer;
		 
		static OIS::InputManager*						inputMgr;
		static OIS::Keyboard*							keyboard;
		static OIS::Mouse*								mouse;
		 
		static Ogre::Camera*							deaultCam; // debug
		 
		static Ogre::FrameEvent							frameEvent; // debug
		 
		static OgreBites::SdkCameraMan*					debugCameraMan; // debug
		static Ogre::SceneManager*						debugSceneMgr; // debug
		 
		static boost::scoped_ptr<Environment::Sky>		defaultSky;

		bool frameRenderingQueued(const Ogre::FrameEvent& evt); // debug

	private:
		void _doMainLoop();
		void _updateOgre(double timesincelastframe);
		void _waitForUserInput();

		typedef boost::scoped_ptr<boost::thread> ThreadPtr;

		static ThreadPtr								_gameConsole;
		static std::deque<std::string>					_commands;
		static boost::mutex								_commandsDequeMutex;
		static bool										_shutdown;
	};
};

#endif
