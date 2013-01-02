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

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

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

		shutdown		= false;
	}

	Application::~Application()
	{
		if(inputmgr) OIS::InputManager::destroyInputSystem(inputmgr);
	}

	bool Application::initOgre(Ogre::String wndtitle, OIS::KeyListener* keylistener, OIS::MouseListener* mouselistener)
	{
		Ogre::LogManager* logmgr = new Ogre::LogManager();

		log = Ogre::LogManager::getSingleton().createLog("logs/general.log", true);
		log->setLogDetail(Ogre::LoggingLevel::LL_BOREME);
		log->setTimeStampEnabled(true);

		root.reset(new Ogre::Root("data/plugins.cfg", "data/engine.cfg", Ogre::StringUtil::BLANK));

		if(!root->showConfigDialog())
			return false;

		renderwnd = root->initialise(true, wndtitle);

		viewport = renderwnd->addViewport(0);
		viewport->setBackgroundColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

		viewport->setCamera(0);

		size_t hwnd = 0;
		OIS::ParamList paramlist;
		renderwnd->getCustomAttribute("WINDOW",& hwnd);

		paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hwnd)));

		inputmgr = OIS::InputManager::createInputSystem(paramlist);

		keyboard = static_cast<OIS::Keyboard*>(inputmgr->createInputObject(OIS::OISKeyboard, true));
		mouse = static_cast<OIS::Mouse*>(inputmgr->createInputObject(OIS::OISMouse, true));

		mouse->getMouseState().height = renderwnd->getHeight();
		mouse->getMouseState().width = renderwnd->getWidth();

		if(keylistener == 0)
			keyboard->setEventCallback(this);
		else
			keyboard->setEventCallback(keylistener);

		if(mouselistener == 0)
			mouse->setEventCallback(this);
		else
			mouse->setEventCallback(mouselistener);

		Ogre::String secName, typeName, archName;
		Ogre::ConfigFile cfgfile;
		cfgfile.load("data/resources.cfg");

		Ogre::ConfigFile::SectionIterator seci = cfgfile.getSectionIterator();
		while (seci.hasMoreElements())
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

		//traymgr.reset(new OgreBites::SdkTrayManager("DefaultTrayMgr", renderwnd, OgreBites::InputContext(), 0));

		timer = new Ogre::Timer();
		timer->reset();

		renderwnd->setActive(true);

		log->logMessage("Application initialized");

		return true;
	}

	void Application::updateOgre(double timeSinceLastFrame)
	{
		return;
	}

	void Application::startLoop()
	{
		log->logMessage("preparing a basic scene...");

		Ogre::Camera* cam;
		Ogre::SceneManager* scenemgr = root->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
		scenemgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

		cam = scenemgr->createCamera("GameCamera");
		cam->setPosition(Ogre::Vector3(5, 60, 60));
		cam->lookAt(Ogre::Vector3(0, 0, 0));
		cam->setNearClipDistance(5);

		cam->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

		viewport->setCamera(cam);

		Ogre::SceneNode* camnode = scenemgr->getRootSceneNode()->createChildSceneNode("MyFirstCameraNode");
		camnode->attachObject(cam);


		Ogre::Entity* head = scenemgr->createEntity("ogrehead.mesh");

		Ogre::SceneNode* lNode = scenemgr->getRootSceneNode()->createChildSceneNode();
		lNode->attachObject(head);

		lNode->setPosition(Ogre::Vector3(0, 0, 0));

		log->logMessage("Main loop entered");

		int timesincelastframe = 1;
		int starttime = 0;

		while(!shutdown)
		{
			if(renderwnd->isClosed()) shutdown = true;

			Ogre::WindowEventUtilities::messagePump();

			if(renderwnd->isActive())
			{
				starttime = timer->getMillisecondsCPU();

				keyboard->capture();
				mouse->capture();

				updateOgre(timesincelastframe);
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

	bool Application::keyPressed(const OIS::KeyEvent& keyeventref)
	{
		if(keyboard->isKeyDown(OIS::KC_SYSRQ))
		{
			renderwnd->writeContentsToTimestampedFile("fairytale_screenshot_", ".jpg");
			return true;
		}

		if(keyboard->isKeyDown(OIS::KC_ESCAPE))
		{
			shutdown = true;
		}

		return true;
	}

	bool Application::keyReleased(const OIS::KeyEvent& keyeventref)
	{
		return true;
	}

	bool Application::mouseMoved(const OIS::MouseEvent& evt)
	{
		return true;
	}

	bool Application::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		return true;
	}

	bool Application::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		return true;
	}
};
