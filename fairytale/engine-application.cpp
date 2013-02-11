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

#include "engine-pch.h"
#include "engine-application.h"
#include "engine-input.h"
#include <Ogre/Overlay/OgreOverlaySystem.h>
#include "game-env-test.h"
#include "util-file.h"
#include "util-procedure.h"
#include <boost/foreach.hpp>

namespace fairytale
{
	void Application::initOgre(const Ogre::String& logFile, const Ogre::String& configFile)
	{
		ONE_OFF_FUNCTION_VOID;

		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		// Init these f*cking singleton classes...
		new Ogre::LogManager();

		core->defaultLog = Ogre::LogManager::getSingleton().createLog(logFile, true);
		core->defaultLog->setLogDetail(Ogre::LoggingLevel::LL_BOREME);
		core->defaultLog->setTimeStampEnabled(true);
	
		core->ogreRoot.reset(new Ogre::Root(Ogre::StringUtil::BLANK, configFile, Ogre::StringUtil::BLANK));

		new Ogre::OverlaySystem();
	}

	bool Application::showConfigDialog()
	{
		ONE_OFF_FUNCTION(bool, false);

		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		return core->ogreRoot->showConfigDialog();
	}

	void Application::initWindow(const Ogre::String& wndtitle)
	{
		ONE_OFF_FUNCTION_VOID;

		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		// Create rendering window
		{
			core->renderWnd = core->ogreRoot->initialise(true, wndtitle);
	
			core->defaultViewport = core->renderWnd->addViewport(0);
			core->defaultViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
		}

		// Init input system
		{
			size_t hWnd = 0;
			OIS::ParamList paramlist;
			core->renderWnd->getCustomAttribute("WINDOW", &hWnd);

			paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
			core->inputMgr = OIS::InputManager::createInputSystem(paramlist);
	
			core->keyboard = static_cast<OIS::Keyboard*>(core->inputMgr->createInputObject(OIS::OISKeyboard, true));
			core->mouse = static_cast<OIS::Mouse*>(core->inputMgr->createInputObject(OIS::OISMouse, true));
	
			core->mouse->getMouseState().height = core->renderWnd->getHeight();
			core->mouse->getMouseState().width = core->renderWnd->getWidth();

			core->keyboard->setEventCallback((OIS::KeyListener*)KeyListenerManager::getInstancePtr());
			core->mouse->setEventCallback((OIS::MouseListener*)MouseListenerManager::getInstancePtr());
		}

		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		core->mainLoopTimer = new Ogre::Timer();
		core->mainLoopTimer->reset();

		core->renderWnd->setActive(true);
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
		}, ONLY_DIRECTORY, false);
	}

	void Application::loadPlugin(const Ogre::String& filename)
	{
		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		core->ogreRoot->loadPlugin(filename);
	}

	void Application::loadPluginsFromDirectory(const std::string& dir)
	{
		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		processFilesInDirectory(dir, [&core](const std::string& plugin) {
			core->ogreRoot->loadPlugin(plugin);
		}, true, ".*\.dll");
	}

	void Application::startLoop()
	{
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);
	
			KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_SYSRQ, [this, &core](const OIS::KeyEvent&) {
				core->renderWnd->writeContentsToTimestampedFile("./screenshots/fairytale_screenshot_", ".png");
			});
			KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_ESCAPE, [this](const OIS::KeyEvent&) {
				shutdown();
			});
		}

		_doMainLoop();
	}

	void Application::_doMainLoop()
	{
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

			core->defaultLog->logMessage("Main loop entered");
			core->_gameConsole.reset(new boost::thread(boost::bind(&Application::_waitForUserInput, this)));

			void doBulletTest();
			doBulletTest();
		}

		int timesincelastframe = 1;
		int starttime = 0;

		CoreMembers* core = CoreMembers::getInstancePtr();

		while(!(core->_shutdown || core->renderWnd->isClosed()))
		{
			// process commands
			{
				boost::mutex::scoped_lock lock(core->_commandsDequeMutex);
				while(!core->_commands.empty())
				{
					try
					{
						PyRun_SimpleString(core->_commands.front().c_str());
						std::cout << ">>> ";
					}
					catch(...)
					{
						if(PyErr_Occurred())
							PyErr_Print();
					}
					core->_commands.pop_front();
				}
			}
			
			{
				Ogre::WindowEventUtilities::messagePump();
	
				starttime = core->mainLoopTimer->getMillisecondsCPU();
	
				core->keyboard->capture();
				core->mouse->capture();
	
				core->ogreRoot->renderOneFrame();
	
				timesincelastframe = core->mainLoopTimer->getMillisecondsCPU() - starttime;
			}
		}

		if(core->inputMgr) OIS::InputManager::destroyInputSystem(core->inputMgr);
		core->defaultLog->logMessage("Main loop quit");
	}

	void Application::_waitForUserInput()
	{
		std::string cmd;
		std::cout << "Console is now available!\n>>> ";
		while(!CoreMembers::getInstancePtr()->_shutdown)
		{
			try
			{
				std::getline(std::cin, cmd);
				LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);
				core->_commands.push_back(cmd);
			}
			catch(...)
			{
				if(PyErr_Occurred())
					PyErr_Print();
			}
			std::cin.clear();
			fflush(stdin);
		}
	}

	void Application::shutdown()
	{
		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		core->_inited ? core->_shutdown = true : exit(0);
	}
};
