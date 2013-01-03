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

namespace fairytale
{
	Application::Application()
	{
		renderwnd		= 0;
		viewport		= 0;
		log				= 0;
		timer			= 0;

		inputmgr		= 0;
		keyboard		= 0;
		mouse			= 0;

		_shutdown		= false;
	}

	Application::~Application()
	{
		// It's said that if you don't do this you'll get a super surprise especially in linux...
		if(inputmgr) OIS::InputManager::destroyInputSystem(inputmgr);

		// Lead the rendering thread to die
		shutdown();
	}

	void Application::initOgre(Ogre::String wndtitle, OIS::KeyListener* keylistener, OIS::MouseListener* mouselistener)
	{
		// Init engine
		{
			Ogre::LogManager* logmgr = new Ogre::LogManager();
	
			log = Ogre::LogManager::getSingleton().createLog("logs/general.log", true);
			log->setLogDetail(Ogre::LoggingLevel::LL_BOREME);
			log->setTimeStampEnabled(true);
	
			root.reset(new Ogre::Root("data/plugins.cfg", "data/engine.cfg", Ogre::StringUtil::BLANK));
	
			// To avoid annoying assertion
			new Ogre::OverlaySystem();
	
			if(!root->showConfigDialog())
				exit(0);
		}

		// Create rendering window
		{
			renderwnd = root->initialise(true, wndtitle);
	
			viewport = renderwnd->addViewport(0);
			viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
		}

		// Init input system
		{
			size_t hwnd = 0;
			OIS::ParamList paramlist;
			renderwnd->getCustomAttribute("WINDOW", &hwnd);

			paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hwnd)));
			inputmgr = OIS::InputManager::createInputSystem(paramlist);
	
			keyboard = static_cast<OIS::Keyboard*>(inputmgr->createInputObject(OIS::OISKeyboard, true));
			mouse = static_cast<OIS::Mouse*>(inputmgr->createInputObject(OIS::OISMouse, true));
	
			mouse->getMouseState().height = renderwnd->getHeight();
			mouse->getMouseState().width = renderwnd->getWidth();

			keyboard->setEventCallback(keylistener);
			mouse->setEventCallback(mouselistener);
		}

		// Load resources
		{
			Ogre::String secName, typeName, archName;
			Ogre::ConfigFile cfgfile;
			cfgfile.load("data/resources.cfg");
	
			Ogre::ConfigFile::SectionIterator seci = cfgfile.getSectionIterator();
			while(seci.hasMoreElements())
			{
				secName = seci.peekNextKey();
				Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
				Ogre::ConfigFile::SettingsMultiMap::iterator i;
				for (i = settings->begin(); i != settings->end(); ++i)
				{
					typeName = i->first;
					archName = i->second;
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
				}
			}
	
			Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
			timer = new Ogre::Timer();
			timer->reset();
		}

		renderwnd->setActive(true);
		log->logMessage("Application initialized");
	}

	void Application::_updateOgre(double timeSinceLastFrame)
	{
		return;
	}

	void Application::startLoop()
	{
		log->logMessage("preparing a basic scene...");
		
		scenemgr = root->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
		scenemgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

		cam = scenemgr->createCamera("GameCamera");
		cam->setFarClipDistance(30000);
		cam->setNearClipDistance(5);

		cam->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

		viewport->setCamera(cam);

		Ogre::SceneNode* camnode = scenemgr->getRootSceneNode()->createChildSceneNode("MyFirstCameraNode");
		camnode->attachObject(cam);


		Ogre::Entity* head = scenemgr->createEntity("ogrehead.mesh");

		Ogre::SceneNode* lNode = scenemgr->getRootSceneNode()->createChildSceneNode();
		lNode->attachObject(head);

		lNode->setPosition(Ogre::Vector3(0, 0, 0));

		mCameraMan = new OgreBites::SdkCameraMan(cam);

		mCameraMan->setTopSpeed(5);

		sky.reset(new Environment::Sky());
		sky->setWeather(mPresets[0]);

		root->addFrameListener(this);

		KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_SYSRQ, [this](const OIS::KeyEvent&) {
			renderwnd->writeContentsToTimestampedFile("fairytale_screenshot_", ".png");
		});

		KeyListenerManager::getInstance().registerListener(KeyListenerManager::KEY_DOWN, OIS::KC_ESCAPE, [this](const OIS::KeyEvent&) {
			shutdown();
		});

		_mainloop.reset(new boost::thread(boost::bind(&Application::_doMainLoop, this)));
		_waitForUserInput();
	}

	void Application::_waitForUserInput()
	{
		std::string cmd;
		while(std::cin >> cmd)
		{
			boost::python::exec(cmd.c_str());
		}
	}

	void Application::_doMainLoop()
	{
		log->logMessage("Main loop entered");

		int timesincelastframe = 1;
		int starttime = 0;

		while(!(_shutdown || renderwnd->isClosed()))
		{
			boost::unique_lock<boost::mutex>(_mutex);

			Ogre::WindowEventUtilities::messagePump();

			if(renderwnd->isActive())
			{
				starttime = timer->getMillisecondsCPU();

				keyboard->capture();
				mouse->capture();

				_updateOgre(timesincelastframe);
				root->renderOneFrame();

				timesincelastframe = timer->getMillisecondsCPU() - starttime;
			}
			else
			{
				boost::this_thread::sleep(boost::posix_time::seconds(1));
			}
		}

		log->logMessage("Main loop quit");
	}

	void Application::shutdown()
	{
		_shutdown = true;
		exit(0);
	}

	bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
		mCameraMan->frameRenderingQueued(evt);
		return true;
	}
};
