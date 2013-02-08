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
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace fairytale
{
	boost::scoped_ptr<Ogre::Root>			Application::ogreRoot;
	Ogre::RenderWindow*						Application::renderWnd = 0;
	Ogre::Viewport*							Application::defaultViewport = 0;
	Ogre::Log*								Application::defaultLog = 0;
	Ogre::Timer*							Application::mainLoopTimer = 0;

	OIS::InputManager*						Application::inputMgr = 0;
	OIS::Keyboard*							Application::keyboard = 0;
	OIS::Mouse*								Application::mouse = 0;

	Ogre::Camera*							Application::deaultCam;

	Ogre::FrameEvent						Application::frameEvent;

	OgreBites::SdkCameraMan*				Application::debugCameraMan;
	Ogre::SceneManager*						Application::debugSceneMgr;

	boost::scoped_ptr<Environment::Sky>		Application::defaultSky;

	Application::ThreadPtr					Application::_gameConsole;
	std::deque<std::string>					Application::_commands;
	boost::mutex							Application::_commandsDequeMutex;
	bool									Application::_shutdown = false;

	void Application::initOgre(const Ogre::String& logFile, const Ogre::String& configFile)
	{
		// Init these f*cking singleton classes...
		new Ogre::LogManager();

		defaultLog = Ogre::LogManager::getSingleton().createLog(logFile, true);
		defaultLog->setLogDetail(Ogre::LoggingLevel::LL_BOREME);
		defaultLog->setTimeStampEnabled(true);
	
		ogreRoot.reset(new Ogre::Root(Ogre::StringUtil::BLANK, configFile, Ogre::StringUtil::BLANK));

		new Ogre::OverlaySystem();
	}

	void Application::showConfigDialog()
	{
		ogreRoot->showConfigDialog();
	}

	void Application::initWindow(const Ogre::String& wndtitle)
	{
		// Create rendering window
		{
			renderWnd = ogreRoot->initialise(true, wndtitle);
	
			defaultViewport = renderWnd->addViewport(0);
			defaultViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
		}

		// Init input system
		{
			size_t hWnd = 0;
			OIS::ParamList paramlist;
			renderWnd->getCustomAttribute("WINDOW", &hWnd);

			paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
			inputMgr = OIS::InputManager::createInputSystem(paramlist);
	
			keyboard = static_cast<OIS::Keyboard*>(inputMgr->createInputObject(OIS::OISKeyboard, true));
			mouse = static_cast<OIS::Mouse*>(inputMgr->createInputObject(OIS::OISMouse, true));
	
			mouse->getMouseState().height = renderWnd->getHeight();
			mouse->getMouseState().width = renderWnd->getWidth();

			keyboard->setEventCallback((OIS::KeyListener*)KeyListenerManager::getInstancePtr());
			mouse->setEventCallback((OIS::MouseListener*)MouseListenerManager::getInstancePtr());
		}

		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		mainLoopTimer = new Ogre::Timer();
		mainLoopTimer->reset();

		renderWnd->setActive(true);
	}

	void Application::addResourceLocation(const Ogre::String& group, const Ogre::String& type, const Ogre::String& archiveName)
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archiveName, type, group);
	}

	void Application::addResourceLocationsFromFile(const Ogre::String& filename)
	{
		using namespace boost::property_tree;
		ptree resourcesFileList;
		json_parser::read_json(filename, resourcesFileList);

		{
			Ogre::String secName, typeName;
			BOOST_FOREACH(const ptree::value_type& sec, resourcesFileList)
			{
				secName = sec.first;
				BOOST_FOREACH(const ptree::value_type& type, sec.second)
				{
					typeName = type.first;
					BOOST_FOREACH(const ptree::value_type& arch, type.second)
					{
						Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch.second.get<std::string>(""), typeName, secName);
					}
				}
			}
		}
	}

	void Application::loadPlugin(const Ogre::String& filename)
	{
		ogreRoot->loadPlugin(filename);
	}

	void Application::loadPluginsFromDirectory(const std::string& dir)
	{
		using namespace boost::filesystem;

		path plugins(dir, native);
		boost::regex filter(".*\.dll");

		if(exists(plugins))
		{
			directory_iterator end;
			boost::smatch what;
			for(directory_iterator iter(plugins); iter != end; ++iter)
			{
				if(is_regular_file(*iter) && boost::regex_match(iter->path().string(), what, filter))
					ogreRoot->loadPlugin(iter->path().string());
			}
		}
	}

	void Application::_updateOgre(double timeSinceLastFrame)
	{
		return;
	}

	void Application::startLoop()
	{
		defaultLog->logMessage("preparing a basic scene...");
		
		debugSceneMgr = ogreRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
		debugSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

		deaultCam = debugSceneMgr->createCamera("GameCamera");
		deaultCam->setFarClipDistance(30000);
		deaultCam->setNearClipDistance(5);

		deaultCam->setAspectRatio(Ogre::Real(defaultViewport->getActualWidth()) / Ogre::Real(defaultViewport->getActualHeight()));

		defaultViewport->setCamera(deaultCam);

		Ogre::SceneNode* camnode = debugSceneMgr->getRootSceneNode()->createChildSceneNode("MyFirstCameraNode");
		camnode->attachObject(deaultCam);


		Ogre::Entity* head = debugSceneMgr->createEntity("ogrehead.mesh");

		Ogre::SceneNode* lNode = debugSceneMgr->getRootSceneNode()->createChildSceneNode();
		lNode->attachObject(head);

		lNode->setPosition(Ogre::Vector3(0, 0, 0));

		debugCameraMan = new OgreBites::SdkCameraMan(deaultCam);

		debugCameraMan->setTopSpeed(5);

		//defaultSky.reset(new Environment::Sky());
		//defaultSky->setWeather(mPresets[1]);

		ogreRoot->addFrameListener(this);

		KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_SYSRQ, [this](const OIS::KeyEvent&) {
			renderWnd->writeContentsToTimestampedFile("screenshots/fairytale_screenshot_", ".png");
		});

		KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_ESCAPE, [this](const OIS::KeyEvent&) {
			shutdown();
		});

		_doMainLoop();
	}

	void Application::_doMainLoop()
	{
		defaultLog->logMessage("Main loop entered");

		_gameConsole.reset(new boost::thread(boost::bind(&Application::_waitForUserInput, this)));

		int timesincelastframe = 1;
		int starttime = 0;

		while(!(_shutdown || renderWnd->isClosed()))
		{
			// process commands
			{
				boost::mutex::scoped_lock lock(_commandsDequeMutex);
				while(!_commands.empty())
				{
					try
					{
						PyRun_SimpleString(_commands.front().c_str());
					}
					catch(...)
					{
						if(PyErr_Occurred())
							PyErr_Print();
					}
					_commands.pop_front();
				}
			}
			
			{
				Ogre::WindowEventUtilities::messagePump();
	
				starttime = mainLoopTimer->getMillisecondsCPU();
	
				keyboard->capture();
				mouse->capture();
	
				_updateOgre(timesincelastframe);
				ogreRoot->renderOneFrame();
	
				timesincelastframe = mainLoopTimer->getMillisecondsCPU() - starttime;
			}
		}

		if(inputMgr) OIS::InputManager::destroyInputSystem(inputMgr);
		defaultLog->logMessage("Main loop quit");
	}

	void Application::_waitForUserInput()
	{
		std::string cmd;
		while(!_shutdown)
		{
			try
			{
				std::cout << ">>> ";
				std::getline(std::cin, cmd);
				boost::mutex::scoped_lock lock(_commandsDequeMutex);
				_commands.push_back(cmd);
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
		_shutdown = true;
	}

	bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
		debugCameraMan->frameRenderingQueued(evt);
		return true;
	}
};
