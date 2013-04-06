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

#ifndef __FAIRYTALE_ENGINE_IMPL_H__
#define __FAIRYTALE_ENGINE_IMPL_H__

#include <string>
#include <boost/function.hpp>

namespace chaiscript {

	class ChaiScript;

}

namespace fairytale { namespace engine {

	class InputManager;
	class GraphicManager;
	class PhysicsWorld;

	// Thread-safe class.
	class Engine
	{
	private:
		struct EngineImpl;
		EngineImpl* _mImpl;

	public:
		Engine();
		~Engine();

		void start();
		void stop();
		void appendGraphicManipulation(const boost::function<void()>& operate);
		void appendScriptCommand(const std::string& cmdLine);
		// DO NOT call this unless YOU are a thread.
		void _processScript();
		InputManager* getInputManager();
		GraphicManager* getGraphicManager();
		PhysicsWorld* getPhysicsWorld();
		chaiscript::ChaiScript* getScript();
	};

} }

#endif
