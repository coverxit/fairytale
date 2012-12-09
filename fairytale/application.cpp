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
		// Viewport Test
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
		window->setAutoUpdated(true);

		// ManualObject Test
		Ogre::ManualObject*	manualobject		= scene->createManualObject("CubeWithAxes");
		// Always tell if you want to update the 3D (vertex/index) later or not. 
		bool				updateitlater		= false;
		manualobject->setDynamic(updateitlater);

		float size = 0.7f;

		manualobject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		float cp = 1.0f * size ;
		float cm = -1.0f * size;
 
		manualobject->position(cm, cp, cm);// a vertex
		manualobject->colour(Ogre::ColourValue(0.0f,1.0f,0.0f,1.0f));
		manualobject->position(cp, cp, cm);// a vertex
		manualobject->colour(Ogre::ColourValue(1.0f,1.0f,0.0f,1.0f));
		manualobject->position(cp, cm, cm);// a vertex
		manualobject->colour(Ogre::ColourValue(1.0f,0.0f,0.0f,1.0f));
		manualobject->position(cm, cm, cm);// a vertex
		manualobject->colour(Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f));
 
		manualobject->position(cm, cp, cp);// a vertex
		manualobject->colour(Ogre::ColourValue(0.0f,1.0f,1.0f,1.0f));
		manualobject->position(cp, cp, cp);// a vertex
		manualobject->colour(Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f));
		manualobject->position(cp, cm, cp);// a vertex
		manualobject->colour(Ogre::ColourValue(1.0f,0.0f,1.0f,1.0f));
		manualobject->position(cm, cm, cp);// a vertex
		manualobject->colour(Ogre::ColourValue(0.0f,0.0f,1.0f,1.0f));

		// face behind / front
		manualobject->triangle(0,1,2);
		manualobject->triangle(2,3,0);
		manualobject->triangle(4,6,5);
		manualobject->triangle(6,4,7);
 
		// face top / down
		manualobject->triangle(0,4,5);
		manualobject->triangle(5,1,0);
		manualobject->triangle(2,6,7);
		manualobject->triangle(7,3,2);
 
		// face left / right
		manualobject->triangle(0,7,4);
		manualobject->triangle(7,0,3);
		manualobject->triangle(1,5,6);
		manualobject->triangle(6,2,1);	

		manualobject->end();

		manualobject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);

		float lAxeSize = 2.0f * size;
		manualobject->position(0.0f, 0.0f, 0.0f);
		manualobject->colour(Ogre::ColourValue::Red);
		manualobject->position(lAxeSize, 0.0f, 0.0f);
		manualobject->colour(Ogre::ColourValue::Red);
		manualobject->position(0.0f, 0.0f, 0.0f);
		manualobject->colour(Ogre::ColourValue::Green);
		manualobject->position(0.0, lAxeSize, 0.0);
		manualobject->colour(Ogre::ColourValue::Green);
		manualobject->position(0.0f, 0.0f, 0.0f);
		manualobject->colour(Ogre::ColourValue::Blue);
		manualobject->position(0.0, 0.0, lAxeSize);
		manualobject->colour(Ogre::ColourValue::Blue);
 
		manualobject->index(0);
		manualobject->index(1);
		manualobject->index(2);
		manualobject->index(3);
		manualobject->index(4);
		manualobject->index(5);

		manualobject->end();

		Ogre::String lNameOfTheMesh = "MeshCubeAndAxe";
		Ogre::String lResourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
		manualobject->convertToMesh(lNameOfTheMesh);
 
		// Now I can create several entities using that mesh
		int lNumberOfEntities = 5;
		for(int iter = 0; iter < lNumberOfEntities; ++iter)
			{
			Ogre::Entity* lEntity = scene->createEntity(lNameOfTheMesh);
			// Now I attach it to a scenenode, so that it becomes present in the scene.
			Ogre::SceneNode* lNode = rootscenenode->createChildSceneNode();
			lNode->attachObject(lEntity);
			// I move the SceneNode so that it is visible to the camera.
			float lPositionOffset = float(1+ iter * 2) - (float(lNumberOfEntities));
			lNode->translate(lPositionOffset, lPositionOffset, -10.0f);
		}

		root->clearEventTimes();
	}

	void Application::StartRendering()
	{
		//while(!window->isClosed())
		//{
		//	window->update(false);
		//	window->swapBuffers();
		//	root->renderOneFrame();
		//	Ogre::WindowEventUtilities::messagePump();
		//}

		// I don't know why do the tutorial author decided to render every frame manually..
		root->startRendering();
	}
};
