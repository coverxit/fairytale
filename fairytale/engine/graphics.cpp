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

#include "pch.h"

//#include <Ogre/Overlay/OgreOverlaySystem.h>

#include "graphics.h"
#include "../util/file.h"

namespace fairytale { namespace engine { namespace graphics {

	boost::scoped_ptr<Ogre::Root>	ogreRoot;
	Ogre::RenderWindow*				renderWnd;
	Ogre::Viewport*					defaultViewport;
	bool							shutdown;
	boost::mutex					ogreMutex;
	
	OGREMutexLock::OGREMutexLock()
	{
		ogreMutex.try_lock();
	}

	OGREMutexLock::~OGREMutexLock()
	{
		ogreMutex.unlock();
	}

	void initOgre()
	{
		ogreRoot.reset(new Ogre::Root(Ogre::StringUtil::BLANK, "engine.ini", "fairytale.log"));
		//new Ogre::OverlaySystem();

		loadPluginsFromDirectory("plugins");

		if(!ogreRoot->showConfigDialog())
			stopRendering();

		renderWnd = ogreRoot->initialise(true, "the world fairytale");
		defaultViewport = renderWnd->addViewport(0);
		defaultViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
		renderWnd->setActive(true);

		shutdown = false;
	}

	void stopRendering()
	{
		shutdown = true;
	}
	
	bool renderingStopped()
	{
		return shutdown;
	}

	Ogre::Root* getOGRE()
	{
		return ogreRoot.get();
	}

	Ogre::RenderWindow* getDefaultRenderWindow()
	{
		return renderWnd;
	}

	Ogre::Viewport* getDefaultViewport()
	{
		return defaultViewport;
	}
	
	void addResourceLocation(const std::string& dir)
	{
		using namespace fairytale::util;

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

	void loadPlugin(const Ogre::String& filename)
	{
		OGREMutexLock();
		ogreRoot->loadPlugin(filename);
	}

	void loadPluginsFromDirectory(const std::string& dir)
	{
		OGREMutexLock();
		util::processFilesInDirectory(dir, [](const std::string& plugin) {
			ogreRoot->loadPlugin(plugin);
		}, true, ".*\.dll");
	}

	void takeScreenshot()
	{
		OGREMutexLock();
		renderWnd->writeContentsToTimestampedFile("fairytale_screenshot_", ".png");
	}

} } }
