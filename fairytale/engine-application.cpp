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
#include "engine-application.h"
#include "engine-input.h"
#include "util-file.h"

namespace fairytale
{
	Application::Application() :
		ogreRoot(0),
		renderWnd(0),
		defaultViewport(0),
		inputMgr(0),
		keyboard(0),
		mouse(0),
		_shutdown(false)
	{
		// Create root
		{
			const_cast<Ogre::Root*>(ogreRoot) = new Ogre::Root(Ogre::StringUtil::BLANK, "engine.ini", "fairytale.log");
			new Ogre::OverlaySystem();

			loadPluginsFromDirectory("plugins");

			if(!ogreRoot->showConfigDialog())
				shutdown();
		}

		// Create rendering window
		{
			const_cast<Ogre::RenderWindow*>(renderWnd) = ogreRoot->initialise(true, "the world fairytale");
			const_cast<Ogre::Viewport*>(defaultViewport) = renderWnd->addViewport(0);
			defaultViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
			renderWnd->setActive(true);
		}

		// Create input system
		{
			size_t hWnd = 0;
			OIS::ParamList paramlist;
			renderWnd->getCustomAttribute("WINDOW", &hWnd);
			paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

			const_cast<OIS::InputManager*>(inputMgr) = OIS::InputManager::createInputSystem(paramlist);
			const_cast<OIS::Keyboard*>(keyboard) = static_cast<OIS::Keyboard*>(inputMgr->createInputObject(OIS::OISKeyboard, true));
			const_cast<OIS::Mouse*>(mouse) = static_cast<OIS::Mouse*>(inputMgr->createInputObject(OIS::OISMouse, true));

			mouse->getMouseState().height = renderWnd->getHeight();
			mouse->getMouseState().width = renderWnd->getWidth();

			keyboard->setEventCallback((OIS::KeyListener*)KeyListenerManager::getInstancePtr());
			mouse->setEventCallback((OIS::MouseListener*)MouseListenerManager::getInstancePtr());
		}
	}

	Application::~Application()
	{
		if(mouse) inputMgr->destroyInputObject(mouse);
		if(keyboard) inputMgr->destroyInputObject(keyboard);
		if(inputMgr) OIS::InputManager::destroyInputSystem(inputMgr);

		delete ogreRoot;
	}

	void Application::startLoop()
	{
		{
			LOCK_AND_GET_INSTANCE_PTR(KeyListenerManager, keymgr);

			keymgr->bindKey(OIS::KC_SYSRQ, KeyListenerManager::DOWN, [this](const OIS::KeyEvent&) {
				takeScreenshot();
			});
			keymgr->bindKey(OIS::KC_ESCAPE, KeyListenerManager::DOWN, [this](const OIS::KeyEvent&) {
				shutdown();
			});
		}

		_doMainLoop();
	}

	void Application::shutdown()
	{
		_shutdown = true;
	}

	void Application::addResourceLocation(const std::string& dir)
	{
		processDirectory(dir, [](const std::string& grouppath) {
			std::string groupdirname(grouppath.substr(grouppath.find_last_of("\\") + 1));

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(grouppath, "FileSystem", groupdirname);

			processDirectory(grouppath, [&groupdirname](const std::string& sourcepath) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sourcepath, "FileSystem", groupdirname);
			}, ONLY_DIRECTORY, true);

			processFilesInDirectory(grouppath, [&groupdirname](const std::string& sourcepath) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sourcepath, "Zip", groupdirname);
			}, true, ".*\.zip");

			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(groupdirname);
		}, ONLY_DIRECTORY, false);
	}

	void Application::loadPlugin(const Ogre::String& filename)
	{
		boost::mutex::scoped_try_lock(_mutex);

		ogreRoot->loadPlugin(filename);
	}

	void Application::loadPluginsFromDirectory(const std::string& dir)
	{
		boost::mutex::scoped_try_lock(_mutex);

		processFilesInDirectory(dir, [this](const std::string& plugin) {
			this->ogreRoot->loadPlugin(plugin);
		}, true, ".*\.dll");
	}

	void Application::takeScreenshot()
	{
		boost::mutex::scoped_try_lock(_mutex);
		renderWnd->writeContentsToTimestampedFile("fairytale_screenshot_", ".png");
	}

	void Application::_doMainLoop()
	{
		Ogre::LogManager::getSingleton().stream() << "Main loop entered";
		_gameConsole.reset(new boost::thread(boost::bind(&Application::_waitForUserInput, this)));

		while(!(_shutdown || renderWnd->isClosed()))
		{
			// process commands
			{
				boost::mutex::scoped_lock(_commandsDequeMutex);
				while(!_commands.empty())
				{
					try
					{
						Sqrat::Script script;
						script.CompileString(_commands.front());
						script.Run();
					}
					catch(Sqrat::Exception& e)
					{
						std::cout << e.Message() << std::endl;
					}
					std::cout << ">>> ";
					_commands.pop_front();
				}
			}

			{
				Ogre::WindowEventUtilities::messagePump();
	
				keyboard->capture();
				mouse->capture();

				ogreRoot->renderOneFrame();
			}
		}

		Ogre::LogManager::getSingleton().stream() << "Main loop quit";
	}

	void Application::_waitForUserInput()
	{
		std::string cmd;
		std::cout << "Console is now available!\n>>> ";
		while(!_shutdown)
		{
			std::getline(std::cin, cmd);
			boost::mutex::scoped_lock(_commandsDequeMutex);
			_commands.push_back(cmd);
			std::cin.clear();
			fflush(stdin);
		}
	}
};
