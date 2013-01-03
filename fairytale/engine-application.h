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
	class Application : public Singleton<Application>, Ogre::FrameListener
	{
	public:
		Application();
		~Application();

		void initOgre(Ogre::String wndtitle, OIS::KeyListener* keylistener, OIS::MouseListener* mouselistener);
		void startLoop();
		void shutdown();
		
		boost::scoped_ptr<Ogre::Root>			root;
		Ogre::RenderWindow*						renderwnd;
		Ogre::Viewport*							viewport;
		Ogre::Log*								log;
		Ogre::Timer*							timer;

		OIS::InputManager*						inputmgr;
		OIS::Keyboard*							keyboard;
		OIS::Mouse*								mouse;

		Ogre::Camera*							cam; // debug

		Ogre::FrameEvent						m_FrameEvent; // debug

		OgreBites::SdkCameraMan*				mCameraMan; // debug
		Ogre::SceneManager*						scenemgr; // debug

		boost::scoped_ptr<Environment::Sky>		sky;

		bool frameRenderingQueued(const Ogre::FrameEvent& evt); // debug

	private:
		void _doMainLoop();
		void _updateOgre(double timesincelastframe);
		void _waitForUserInput();

		typedef boost::scoped_ptr<boost::thread> ThreadPtr;

		ThreadPtr						_mainloop;
		bool							_shutdown;
	};
};

#endif
