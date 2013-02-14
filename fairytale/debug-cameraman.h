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
#include <SdkCameraMan.h>

namespace fairytale
{
	class DebugCameraMan : public OgreBites::SdkCameraMan, public OIS::KeyListener, public OIS::MouseListener, public Ogre::FrameListener
	{
	public:
		DebugCameraMan(Ogre::Camera* cam) : SdkCameraMan(cam) {}

		bool mouseMoved(const OIS::MouseEvent& evt)
		{
			SdkCameraMan::injectMouseMove(evt);
			return true;
		}

		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			SdkCameraMan::injectMouseDown(evt, id);
			return true;
		}

		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			SdkCameraMan::injectMouseUp(evt, id);
			return true;
		}

		bool keyPressed(const OIS::KeyEvent& evt)
		{
			SdkCameraMan::injectKeyDown(evt);
			return true;
		}

		bool keyReleased(const OIS::KeyEvent& evt)
		{
			SdkCameraMan::injectKeyUp(evt);
			return true;
		}

		bool frameRenderingQueued(const Ogre::FrameEvent& evt)
		{
			SdkCameraMan::frameRenderingQueued(evt);
			return true;
		}
	};

	class DebugCameraManFrameListener : public Ogre::FrameListener
	{
	protected:
		boost::shared_ptr<DebugCameraMan> _camman;

	public:
		DebugCameraManFrameListener(const boost::shared_ptr<DebugCameraMan>& camman) : _camman(camman) {}

		bool frameRenderingQueued(const Ogre::FrameEvent& evt)
		{
			_camman->frameRenderingQueued(evt);
			return true;
		}
	};
}
