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

#include <boost/scoped_ptr.hpp>
#include <boost/serialization/singleton.hpp>

#include "engine-pch.h"

namespace fairytale
{
	class Application : public boost::serialization::singleton<Application>, OIS::KeyListener, OIS::MouseListener
	{
	public:
		Application();
		~Application();

		bool initOgre(Ogre::String wndtitle, OIS::KeyListener* keylistener = 0, OIS::MouseListener* mouselistener = 0);
		void updateOgre(double timesincelastframe);
		void startLoop();

		bool keyPressed(const OIS::KeyEvent& keyeventref);
		bool keyReleased(const OIS::KeyEvent& keyeventref);

		bool mouseMoved(const OIS::MouseEvent& evt);
		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

		boost::scoped_ptr<Ogre::Root>					root;
		Ogre::RenderWindow*								renderwnd;
		Ogre::Viewport*									viewport;
		Ogre::Log*										log;
		Ogre::Timer*									timer;

		OIS::InputManager*								inputmgr;
		OIS::Keyboard*									keyboard;
		OIS::Mouse*										mouse;

		bool											shutdown;
	};
};

#endif
