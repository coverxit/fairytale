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

#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/scoped_ptr.hpp>

#include "input.h"

namespace fairytale { namespace engine {

	struct InputManager::InputManagerImpl : public OIS::KeyListener, public OIS::MouseListener
	{
		OIS::InputManager*	inputMgr;
		OIS::Keyboard*		keyboard;
		OIS::Mouse*			mouse;

		struct _KeyBindingImpl
		{
			KeyState keyState;
			boost::function<void(const OIS::KeyEvent&)> callback;
			std::vector<OIS::KeyCode> withNonCharKeys;
		};

		typedef boost::unordered_map<boost::uuids::uuid, boost::shared_ptr<_KeyBindingImpl>> KeyBindingImplMap;
		boost::unordered_map<OIS::KeyCode, KeyBindingImplMap> keyBindings;
		boost::unordered_map<size_t, boost::shared_ptr<OIS::KeyListener>> keyListeners;
		boost::unordered_map<size_t, boost::shared_ptr<OIS::MouseListener>> mouseListeners;

		boost::mutex keyBindingMutex, keyListenerMutex, mouseListenerMutex, messagePumpMutex;

		boost::thread_group messageProcessThreads;

		InputManagerImpl() : inputMgr(0), keyboard(0), mouse(0) {}
		~InputManagerImpl()
		{
			if(mouse) inputMgr->destroyInputObject(mouse);
			if(keyboard) inputMgr->destroyInputObject(keyboard);
			if(inputMgr) OIS::InputManager::destroyInputSystem(inputMgr);
		}

		bool keyPressed(const OIS::KeyEvent& evt)
		{
			{
				boost::lock_guard<boost::mutex>(this->keyBindingMutex);

				auto possibleBindings(keyBindings.find(evt.key));
				if(possibleBindings != keyBindings.end())
				{
					for(auto iter(possibleBindings->second.begin()); iter != possibleBindings->second.end(); ++iter)
					{
						if(iter->second->keyState != KeyState::DOWN)
							continue;

						bool withKeyAllPressed = true;
						for(auto withKeysIter(iter->second->withNonCharKeys.begin());
							withKeyAllPressed && (withKeysIter != iter->second->withNonCharKeys.end()); ++withKeysIter)
						{
							if(!keyboard->isKeyDown((*withKeysIter)))
								withKeyAllPressed = false;
						}

						if(withKeyAllPressed)
							iter->second->callback(evt);
					}
				}
			}

			{
				boost::lock_guard<boost::mutex>(this->keyListenerMutex);

				for(auto iter(keyListeners.begin()); iter != keyListeners.end(); ++iter)
					iter->second->keyPressed(evt);
			}

			return true;
		}

		bool keyReleased(const OIS::KeyEvent& evt)
		{
			{
				boost::lock_guard<boost::mutex>(this->keyBindingMutex);

				auto possibleBindings(keyBindings.find(evt.key));
				if(possibleBindings != keyBindings.end())
				{
					for(auto iter(possibleBindings->second.begin()); iter != possibleBindings->second.end(); ++iter)
					{
						if(iter->second->keyState != KeyState::UP)
							continue;

						bool withKeyAllReleased = true;
						for(auto withKeysIter(iter->second->withNonCharKeys.begin());
							withKeyAllReleased && (withKeysIter != iter->second->withNonCharKeys.end()); ++withKeysIter)
						{
							if(keyboard->isKeyDown((*withKeysIter)))
								withKeyAllReleased = false;
						}

						if(withKeyAllReleased)
							iter->second->callback(evt);
					}
				}
			}

			{
				boost::lock_guard<boost::mutex>(this->keyListenerMutex);

				for(auto iter(keyListeners.begin()); iter != keyListeners.end(); ++iter)
					iter->second->keyReleased(evt);
			}

			return true;
		}

		bool mouseMoved(const OIS::MouseEvent& evt)
		{
			boost::lock_guard<boost::mutex>(this->mouseListenerMutex);

			for(auto iter(mouseListeners.begin()); iter != mouseListeners.end(); ++iter)
			{
				iter->second->mouseMoved(evt);
			}

			return true;
		}

		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			boost::lock_guard<boost::mutex>(this->mouseListenerMutex);

			for(auto iter(mouseListeners.begin()); iter != mouseListeners.end(); ++iter)
			{
				iter->second->mousePressed(evt, id);
			}

			return true;
		}

		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			boost::lock_guard<boost::mutex>(this->mouseListenerMutex);

			for(auto iter(mouseListeners.begin()); iter != mouseListeners.end(); ++iter)
			{
				iter->second->mouseReleased(evt, id);
			}

			return true;
		}
	};
	
	InputManager::InputManager(size_t windowHandle) : _mImpl(new InputManagerImpl)
	{
		OIS::ParamList paramlist;
		paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(windowHandle)));

		_mImpl->inputMgr = OIS::InputManager::createInputSystem(paramlist);
		_mImpl->keyboard = static_cast<OIS::Keyboard*>(_mImpl->inputMgr->createInputObject(OIS::OISKeyboard, true));
		_mImpl->mouse = static_cast<OIS::Mouse*>(_mImpl->inputMgr->createInputObject(OIS::OISMouse, true));

		_mImpl->keyboard->setEventCallback(_mImpl);
		_mImpl->mouse->setEventCallback(_mImpl);
	}

	InputManager::~InputManager()
	{
		delete _mImpl;
	}

	void InputManager::setMouseBufferedArea(std::pair<int, int> widthAndHeight)
	{
		_mImpl->mouse->getMouseState().width = widthAndHeight.first;
		_mImpl->mouse->getMouseState().height = widthAndHeight.second;
	}

	boost::shared_ptr<InputManager::KeyBinding> InputManager::registerKeyBinding(
		OIS::KeyCode keyCode, KeyState state,
		const boost::function<void(const OIS::KeyEvent&)>& callback,
		const std::vector<OIS::KeyCode>& withNonCharKeys)
	{
		boost::shared_ptr<InputManagerImpl::_KeyBindingImpl> newBinding(new InputManagerImpl::_KeyBindingImpl);

		newBinding->keyState = state;
		newBinding->callback = callback;
		newBinding->withNonCharKeys = withNonCharKeys;

		boost::shared_ptr<KeyBinding> bindingId(new KeyBinding);

		bindingId->keyCode = keyCode;
		bindingId->bindingId = boost::uuids::random_generator()();

		{
			boost::lock_guard<boost::mutex>(_mImpl->keyBindingMutex);

			_mImpl->keyBindings[keyCode].insert(std::make_pair(bindingId->bindingId, newBinding));
		}

		return bindingId;
	}

	bool InputManager::unregisterKeyBinding(const boost::shared_ptr<KeyBinding>& bindingInfo)
	{
		boost::lock_guard<boost::mutex>(_mImpl->keyBindingMutex);

		auto keyIter(_mImpl->keyBindings.find(bindingInfo->keyCode));
		if(keyIter == _mImpl->keyBindings.end())
			return false;

		if(keyIter->second.erase(bindingInfo->bindingId) == 0)
			return false;

		if(keyIter->second.empty())
			_mImpl->keyBindings.erase(keyIter);

		return true;
	}

	bool InputManager::registerKeyListener(const boost::shared_ptr<OIS::KeyListener>& listener)
	{
		boost::lock_guard<boost::mutex>(_mImpl->keyListenerMutex);

		return _mImpl->keyListeners.insert(std::make_pair((size_t)listener.get(), listener)).second;
	}

	bool InputManager::unregisterKeyListener(const boost::shared_ptr<OIS::MouseListener>& listener)
	{
		boost::lock_guard<boost::mutex>(_mImpl->keyListenerMutex);

		return _mImpl->keyListeners.erase((size_t)listener.get()) != 0;
	}

	bool InputManager::registerMouseListener(const boost::shared_ptr<OIS::MouseListener>& listener)
	{
		boost::lock_guard<boost::mutex>(_mImpl->mouseListenerMutex);

		return _mImpl->mouseListeners.insert(std::make_pair((size_t)listener.get(), listener)).second;
	}

	bool InputManager::unregisterMouseListener(const boost::shared_ptr<OIS::MouseListener>& listener)
	{
		boost::lock_guard<boost::mutex>(_mImpl->mouseListenerMutex);

		return _mImpl->mouseListeners.erase((size_t)listener.get()) != 0;
	}

	void InputManager::processOneInputFrame()
	{
		{
			boost::lock_guard<boost::mutex>(_mImpl->messagePumpMutex);

			Ogre::WindowEventUtilities::messagePump();
		}
		
		_mImpl->keyboard->capture();
		_mImpl->mouse->capture();
	}
} }
