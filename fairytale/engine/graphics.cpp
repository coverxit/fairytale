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

#include <queue>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include "graphics.h"
#include "../util/file.h"

namespace fairytale { namespace engine {

	struct GraphicManager::GraphicManagerImpl
	{
		boost::scoped_ptr<Ogre::Root>		ogre;
		Ogre::RenderWindow*					window;
		Ogre::Viewport*						viewport;
		std::queue<boost::function<void()>>	operationsNextFrame;
		boost::mutex						operationQueueMutex;
	};

	GraphicManager::GraphicManager() : _mImpl(new GraphicManagerImpl)
	{
		_mImpl->ogre.reset(new Ogre::Root(Ogre::StringUtil::BLANK, "engine.ini", "fairytale.log"));
		//new Ogre::OverlaySystem();

		_loadPluginsFromDirectory("plugins");

		if(!_mImpl->ogre->showConfigDialog())
			exit(EXIT_FAILURE);

		_mImpl->window = _mImpl->ogre->initialise(true, "the world fairytale");
		_mImpl->viewport = _mImpl->window->addViewport(0);
		_mImpl->viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
		_mImpl->window->setActive(true);
	}

	GraphicManager::~GraphicManager()
	{
		delete _mImpl;
	}

	void GraphicManager::renderOneFrame()
	{
		{
			boost::lock_guard<boost::mutex>(_mImpl->operationQueueMutex);

			while(!_mImpl->operationsNextFrame.empty())
			{
				_mImpl->operationsNextFrame.front()();
				_mImpl->operationsNextFrame.pop();
			}
		}
		
		_mImpl->ogre->renderOneFrame();
	}

	void GraphicManager::appendEngineManipulation(const boost::function<void()>& operate)
	{
		boost::lock_guard<boost::mutex>(_mImpl->operationQueueMutex);
		_mImpl->operationsNextFrame.push(operate);
	}

	void GraphicManager::addFrameListener(Ogre::FrameListener* listener)
	{
		_mImpl->ogre->addFrameListener(listener);
	}

	void GraphicManager::removeFrameListener(Ogre::FrameListener* listener)
	{
		_mImpl->ogre->removeFrameListener(listener);
	}

	size_t GraphicManager::getRenderWindowHandle()
	{
		size_t hWnd = 0;
		_mImpl->window->getCustomAttribute("WINDOW", &hWnd);
		return hWnd;
	}

	void GraphicManager::addResourceLocation(const std::string& dir)
	{
		using namespace fairytale::util;

		processDirectory(dir, [](const std::string& grouppath) {
			std::string groupdirname(grouppath.substr(grouppath.find_last_of("\\") + 1));

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(grouppath, "FileSystem", groupdirname);

			processDirectory(grouppath, [&groupdirname](const std::string& sourcepath) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sourcepath, "FileSystem", groupdirname);
			}, DirectoryProcessMode::ONLY_DIRECTORY, true);

			processFilesInDirectory(grouppath, [&groupdirname](const std::string& sourcepath) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sourcepath, "Zip", groupdirname);
			}, true, ".*\.zip");

			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(groupdirname);
		}, DirectoryProcessMode::ONLY_DIRECTORY, false);
	}

	void GraphicManager::_loadPlugin(const std::string& fileName)
	{
		_mImpl->ogre->loadPlugin(fileName);
	}

	void GraphicManager::_loadPluginsFromDirectory(const std::string& dir)
	{
		util::processFilesInDirectory(dir, [this](const std::string& plugin) {
			_mImpl->ogre->loadPlugin(plugin);
		}, true, ".*\.dll");
	}

	void GraphicManager::takeScreenshot()
	{
		_mImpl->window->writeContentsToTimestampedFile("fairytale_screenshot_", ".png");
	}

	std::pair<int, int> GraphicManager::getRenderWindowWidthAndHeight()
	{
		return std::pair<int, int>(_mImpl->window->getWidth(), _mImpl->window->getHeight());
	}

	float GraphicManager::getViewportAspectRatio()
	{
		return (double)_mImpl->viewport->getActualWidth() / (double)_mImpl->viewport->getActualHeight();
	}

	Ogre::Root* GraphicManager::getOgreRoot()
	{
		return _mImpl->ogre.get();
	}

	void GraphicManager::setCamera(Ogre::Camera* cam)
	{
		_mImpl->viewport->setCamera(cam);
	}

	void GraphicManager::addWindowEventListener(Ogre::WindowEventListener* listener)
	{
		Ogre::WindowEventUtilities::addWindowEventListener(_mImpl->window, listener);
	}

	void GraphicManager::removeWindowEventListener(Ogre::WindowEventListener* listener)
	{
		Ogre::WindowEventUtilities::removeWindowEventListener(_mImpl->window, listener);
	}

} }
