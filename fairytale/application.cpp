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

#include "pch.h"
#include "application.h"

namespace fairytale
{
	void Application::InitEngine(
		const std::string& pluginsfilename,
		const std::string& configfilename,
		const std::string& logfilename)
	{
		// Create Ogre::Root object with specified config files.
		// I think plugins will be loaded automatically...
		root.reset(new Ogre::Root(
			pluginsfilename, configfilename, logfilename));

		// I 
		if(root->showConfigDialog())
		{
			window.reset(root->initialise(true, "fairytale"));
		} else {
			throw std::runtime_error("Unable to create the rendering window.");
		}
	}

	void Application::SetupScene()
	{
		Ogre::SceneManager*	scene				= root->createSceneManager(Ogre::ST_GENERIC, "TestSceneManager");
		Ogre::SceneNode*	rootscenenode		= scene->getRootSceneNode();
		Ogre::Camera*		camera				= scene->createCamera("TestCamera");
		Ogre::SceneNode*	cameranode			= rootscenenode->createChildSceneNode("TestCameraNode");

		cameranode->attachObject(camera);

		float				viewportwidth		= 0.88f;
		float				viewportheight		= 0.88f;
		float				viewportleft		= (1.0f - viewportwidth) * 0.5f;
		float				viewporttop			= (1.0f - viewportheight) * 0.5f;
		unsigned short		mainviewportzorder	= 100;
		Ogre::Viewport*		viewport			= window->addViewport(camera, mainviewportzorder, viewportleft, viewporttop, viewportwidth, viewportheight);

		viewport->setAutoUpdated(true);
		viewport->setBackgroundColour(Ogre::ColourValue(1,0,1));

		float				ratio				= float(viewport->getActualWidth()) / float(viewport->getActualHeight());

		camera->setAspectRatio(ratio);
		camera->setNearClipDistance(1.5f);
		camera->setFarClipDistance(3000.0f); 

		window->setActive(true);
		window->setAutoUpdated(false);

		root->clearEventTimes();
	}

	void Application::StartRendering()
	{
		while(!window->isClosed())
		{
			window->update(false);
			window->swapBuffers();
			root->renderOneFrame();
			Ogre::WindowEventUtilities::messagePump();
		}
	}

};
