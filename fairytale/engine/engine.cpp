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

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <queue>
#include <boost/atomic.hpp>
#include <chrono>

#include "engine.h"
#include "graphics.h"
#include "input.h"
#include "physics.h"

namespace fairytale { namespace engine {

	struct Engine::EngineImpl : public Ogre::WindowEventListener
	{
		boost::scoped_ptr<GraphicManager>	graphic;
		boost::scoped_ptr<InputManager>		input;
		boost::scoped_ptr<PhysicsWorld>		physics;
		boost::scoped_ptr<chaiscript::ChaiScript> script;
		boost::thread_group					threadGroup;

		boost::atomic<bool>					shutdownCalled;

		std::queue<std::string>				scriptCmdQueue;
		boost::shared_mutex					scriptQueueMutex;
		boost::condition_variable			scriptConditionVar;

		std::chrono::system_clock::time_point	lastPhysicsFrameTime;

		EngineImpl() :
			graphic(new GraphicManager),
			input(new InputManager(graphic->getRenderWindowHandle())),
			physics(new PhysicsWorld),
			script(new chaiscript::ChaiScript),
			lastPhysicsFrameTime(std::chrono::high_resolution_clock::now())
		{}

		void windowResized(Ogre::RenderWindow* rw)
		{
			input->setMouseBufferedArea(graphic->getRenderWindowWidthAndHeight());
		}
	};

	Engine::Engine() : _mImpl(new EngineImpl) {}

	Engine::~Engine()
	{
		delete _mImpl;
	}

	void Engine::stop()
	{
		_mImpl->shutdownCalled = true;
	}

	void Engine::start()
	{
		_mImpl->shutdownCalled = false;

		// Physics thread
		auto phyThread = _mImpl->threadGroup.create_thread([this]() {
			while(!_mImpl->shutdownCalled)
			{
				auto now = std::chrono::high_resolution_clock::now();
				// Unit is second.
				double duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _mImpl->lastPhysicsFrameTime).count() / 1000.0;
				_mImpl->lastPhysicsFrameTime = now;
				_mImpl->physics->calculateOnePhysicsFrame((float)duration);
			}
		});

		// Script thread
		auto scriptThread = _mImpl->threadGroup.create_thread([this]() {
			while(!_mImpl->shutdownCalled)
			{
				boost::mutex::scoped_lock lock(_mImpl->scriptQueueMutex);

				_mImpl->scriptConditionVar.wait(lock);

				while(!_mImpl->scriptCmdQueue.empty())
				{
					_mImpl->script->eval(_mImpl->scriptCmdQueue.front());
					_mImpl->scriptCmdQueue.pop();
				}
			}
		});

		while(!_mImpl->shutdownCalled)
		{
			_mImpl->input->processOneInputFrame();
			_mImpl->graphic->renderOneFrame();
		}

		_mImpl->threadGroup.remove_thread(phyThread);
		_mImpl->threadGroup.remove_thread(scriptThread);
	}

	InputManager* Engine::getInputManager()
	{
		return _mImpl->input.get();
	}

	GraphicManager* Engine::getGraphicManager()
	{
		return _mImpl->graphic.get();
	}

	PhysicsWorld* Engine::getPhysicsWorld()
	{
		return _mImpl->physics.get();
	}

	chaiscript::ChaiScript* Engine::getScript()
	{
		return _mImpl->script.get();
	}

	void Engine::postScriptCommand(const std::string& cmdLine)
	{
		boost::mutex::scoped_lock lock(_mImpl->scriptQueueMutex);
		_mImpl->scriptCmdQueue.push(cmdLine);
		lock.unlock();
		_mImpl->scriptConditionVar.notify_all();
	}

	void Engine::postGraphicOperation(const boost::function<void()>& opreation)
	{
		_mImpl->graphic->postOperation(opreation);
	}

	void Engine::postPhysicsOperation(const boost::function<void()>& opreation)
	{
		_mImpl->physics->postOperation(opreation);
	}

} }
