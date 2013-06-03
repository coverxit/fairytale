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

#ifndef __FAIRYTALE_ENGINE_GRAPHICS_H__
#define __FAIRYTALE_ENGINE_GRAPHICS_H__

#include <string>
#include <boost/function.hpp>
#include <utility>

namespace Ogre {

	class Root;
	class FrameListener;
	class Camera;
	class WindowEventListener;

}

namespace fairytale { namespace engine {

	class GraphicManager
	{
	private:
		struct GraphicManagerImpl;
		GraphicManagerImpl* _mImpl;
		void _loadPlugin(const std::string& fileName);
		void _loadPluginsFromDirectory(const std::string& dir);

	public:
		GraphicManager();
		~GraphicManager();

		void renderOneFrame();

		void addFrameListener(Ogre::FrameListener* listener);
		void removeFrameListener(Ogre::FrameListener* listener);
		size_t getRenderWindowHandle();
		std::pair<int, int> getRenderWindowWidthAndHeight();
		float getViewportAspectRatio();
		// Thread-safe function. Append an operate to graphic system that will be done before rendering next frame.
		void postOperation(const boost::function<void()>& operation);
		void addResourceLocation(const std::string& dir);
		void takeScreenshot();
		Ogre::Root* getOgreRoot();
		void setCamera(Ogre::Camera* cam);
		void addWindowEventListener(Ogre::WindowEventListener* listener);
		void removeWindowEventListener(Ogre::WindowEventListener* listener);
	};

} }

#endif
