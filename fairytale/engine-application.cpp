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
#include "util-file.h"
#include "util-procedure.h"

namespace fairytale
{
	class BulletFrameListener : public Ogre::FrameListener
	{
		bool frameStarted(const Ogre::FrameEvent &evt)
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

			//Update Bullet world. Don't forget the debugDrawWorld() part!
			core->phyWorld->stepSimulation(evt.timeSinceLastFrame, 10);
			core->phyWorld->debugDrawWorld();

			//Shows debug if F3 key down.
			core->dbgDraw->setDebugMode(CoreMembers::getInstancePtr()->keyboard->isKeyDown(OIS::KC_F3));
			core->dbgDraw->step();

			return true;
		}
	};

	CoreMembers::CoreMembers()
	{
		renderWnd		= 0;
		defaultViewport	= 0;

		inputMgr		= 0;
		keyboard		= 0;
		mouse			= 0;

		_inited			= false;
		_shutdown		= false;

		_defaultLog		= 0;
	}

	CoreMembers::~CoreMembers()
	{
		dbgDraw.reset();
		phyWorld.reset();

		solver.reset();
		dispatcher.reset();
		collisionConfig.reset();
		broadPhase.reset();

		if(mouse) inputMgr->destroyInputObject(mouse);
		if(keyboard) inputMgr->destroyInputObject(keyboard);
		if(inputMgr) OIS::InputManager::destroyInputSystem(inputMgr);

		ogreRoot.reset();
	}

	void Application::initOgre(const Ogre::String& logFile, const Ogre::String& configFile)
	{
		ONE_OFF_FUNCTION_VOID;

		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

		// Init these f*cking singleton classes...
		new Ogre::LogManager();

		core->_defaultLog = Ogre::LogManager::getSingleton().createLog(logFile, true);
		core->_defaultLog->setLogDetail(Ogre::LoggingLevel::LL_BOREME);
		core->_defaultLog->setTimeStampEnabled(true);
		Ogre::LogManager::getSingleton().setDefaultLog(core->_defaultLog);

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

			//Bullet initialisation.
			core->broadPhase.reset(new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024));
			core->collisionConfig.reset(new btDefaultCollisionConfiguration());
			core->dispatcher.reset(new btCollisionDispatcher(core->collisionConfig.get()));
			core->solver.reset(new btSequentialImpulseConstraintSolver());

			core->phyWorld.reset(new btDiscreteDynamicsWorld(core->dispatcher.get(), core->broadPhase.get(), core->solver.get(), core->collisionConfig.get()));
			core->phyWorld->setGravity(btVector3(0,-9.8,0));

			core->ogreRoot->addFrameListener(new BulletFrameListener());
		}

		{
			LOCK_AND_GET_INSTANCE_PTR(KeyListenerManager, keymgr);

			keymgr->bindKey(OIS::KC_SYSRQ, KeyListenerManager::DOWN, [](const OIS::KeyEvent&) {
				LOCK_AND_GET_INSTANCE_PTR(CoreMembers, _core);
				_core->renderWnd->writeContentsToTimestampedFile("./screenshots/fairytale_screenshot_", ".png");
			});
			keymgr->bindKey(OIS::KC_ESCAPE, KeyListenerManager::DOWN, [this](const OIS::KeyEvent&) {
				shutdown();
			});
		}

		_doMainLoop();
	}

	void Application::_doMainLoop()
	{
		{
			LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

			Ogre::LogManager::getSingleton().stream() << "Main loop entered";
			core->_gameConsole.reset(new boost::thread(boost::bind(&Application::_waitForUserInput, this)));
		}

		void doBulletTest();
		doBulletTest();

		CoreMembers* coreptr = CoreMembers::getInstancePtr();

		while(!(coreptr->_shutdown || coreptr->renderWnd->isClosed()))
		{
			// process commands
			{
				boost::mutex::scoped_lock lock(coreptr->_commandsDequeMutex);
				while(!coreptr->_commands.empty())
				{
					try
					{
						Sqrat::Script script;
						script.CompileString(coreptr->_commands.front());
						script.Run();
					}
					catch(Sqrat::Exception& e)
					{
						std::cout << e.Message() << std::endl;
					}
					std::cout << ">>> ";
					coreptr->_commands.pop_front();
				}
			}

			{
				Ogre::WindowEventUtilities::messagePump();
	
				coreptr->keyboard->capture();
				coreptr->mouse->capture();
	
				coreptr->ogreRoot->renderOneFrame();
			}
		}

		Ogre::LogManager::getSingleton().stream() << "Main loop quit";
	}

	void Application::_waitForUserInput()
	{
		std::string cmd;
		std::cout << "Console is now available!\n>>> ";
		while(!CoreMembers::getInstancePtr()->_shutdown)
		{
			std::getline(std::cin, cmd);
			CoreMembers* coreptr = CoreMembers::getInstancePtr();
			boost::mutex::scoped_lock lock(coreptr->_commandsDequeMutex);
			coreptr->_commands.push_back(cmd);
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
