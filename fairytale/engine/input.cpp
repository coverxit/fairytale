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

#include "input.h"

namespace fairytale { namespace engine { namespace input {

	OIS::InputManager*		inputMgr = 0;
	OIS::Keyboard*			keyboard = 0;
	OIS::Mouse*				mouse = 0;

	class OISDestructor
	{
	public:
		~OISDestructor()
		{
			if(mouse) inputMgr->destroyInputObject(mouse);
			if(keyboard) inputMgr->destroyInputObject(keyboard);
			if(inputMgr) OIS::InputManager::destroyInputSystem(inputMgr);
		}
	};

	static OISDestructor autoOISDestructor;

	class KeyListenerManager : public OIS::KeyListener
	{
	public:
		struct KeyBindingKey
		{
			std::string name;
			std::pair<OIS::KeyCode, KeyState> keycode;

			KeyBindingKey(const std::string& name, std::pair<OIS::KeyCode, KeyState> keycode) :
				name(name), keycode(keycode) {}

			bool operator<(const KeyBindingKey& right) const
			{
				// I want to impl a dual-key map. when name is empty, keycode will be map's key, vice versa.
				if(right.name.empty())
					return keycode < right.keycode;
				else if(right.keycode.first == 0 && right.keycode.second == 0)
					return name < right.name;
				else
					return keycode < right.keycode && name < right.name;
			}
		};

		typedef boost::function<void(const OIS::KeyEvent&)> KeyHandler;

		bool keyPressed(const OIS::KeyEvent& evt)
		{
			KeyActionMap::iterator actioniter(_keybinds.find(KeyBindingKey(std::string(), std::make_pair(evt.key, DOWN))));
			if(actioniter != _keybinds.end())
			{
				actioniter->second(evt);
			}

			for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
			{
				iter->second->keyPressed(evt);
			}

			return true;
		}

		bool keyReleased(const OIS::KeyEvent& evt)
		{
			KeyActionMap::iterator actioniter(_keybinds.find(KeyBindingKey(std::string(), std::make_pair(evt.key, UP))));
			if(actioniter != _keybinds.end())
			{
				actioniter->second(evt);
			}

			for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
			{
				iter->second->keyReleased(evt);
			}

			return true;

		}

		bool registerKeyBinding(const std::string& name, OIS::KeyCode keycode, KeyState state, const boost::function<void(const OIS::KeyEvent&)>& action)
		{
			if(name.empty())
				return false;

			return _keybinds.insert(std::make_pair(KeyBindingKey(name, std::make_pair(keycode, state)), action)).second;
		}

		bool unregisterKeyBinding(const std::string& name)
		{
			return _keybinds.erase((KeyBindingKey(name, std::make_pair(OIS::KeyCode::KC_UNASSIGNED, NONE))));
		}

		bool registerListener(const std::string& name, boost::shared_ptr<OIS::KeyListener> listener)
		{
			return _general.insert(std::make_pair(name, listener)).second;
		}

		bool unregisterListener(const std::string& name)
		{
			return _general.erase(name);
		}

	private:
		typedef std::map<KeyBindingKey, KeyHandler> KeyActionMap;
		typedef std::unordered_map<std::string, boost::shared_ptr<OIS::KeyListener>> ListenerMap;

		ListenerMap _general;
		KeyActionMap _keybinds;
	};

	static KeyListenerManager keyListenerManager;

	class MouseListenerManager : public OIS::MouseListener
	{
	public:
		bool mouseMoved(const OIS::MouseEvent& evt)
		{
			for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
			{
				iter->second->mouseMoved(evt);
			}

			return true;
		}

		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
			{
				iter->second->mousePressed(evt, id);
			}

			return true;
		}

		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
			{
				iter->second->mouseReleased(evt, id);
			}

			return true;
		}

		bool registerListener(const std::string& name, boost::shared_ptr<OIS::MouseListener> listener)
		{
			return _general.insert(std::make_pair(name, listener)).second;
		}

		bool unregisterListener(const std::string& name)
		{
			return _general.erase(name);
		}

	private:
		typedef std::unordered_map<std::string, boost::shared_ptr<OIS::MouseListener>> ListenerMap;

		ListenerMap _general;
	};

	static MouseListenerManager mouseListenerManager;

	void initOIS(size_t hWindow)
	{
		OIS::ParamList paramlist;
		paramlist.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWindow)));

		inputMgr = OIS::InputManager::createInputSystem(paramlist);
		keyboard = static_cast<OIS::Keyboard*>(inputMgr->createInputObject(OIS::OISKeyboard, true));
		mouse = static_cast<OIS::Mouse*>(inputMgr->createInputObject(OIS::OISMouse, true));

		keyboard->setEventCallback(&keyListenerManager);
		mouse->setEventCallback(&mouseListenerManager);
	}

	void setMouseBufferedArea(int width, int height)
	{
		mouse->getMouseState().width = width;
		mouse->getMouseState().height = height;
	}

	OIS::Keyboard* getKeyboard()
	{
		return keyboard;
	}

	OIS::Mouse* getMouse()
	{
		return mouse;
	}

	bool registerKeyBinding(const std::string& name, OIS::KeyCode keycode, KeyState state, const boost::function<void(const OIS::KeyEvent&)>& action)
	{
		return keyListenerManager.registerKeyBinding(name, keycode, state, action);
	}

	bool unregisterKeyBinding(const std::string& name)
	{
		return keyListenerManager.unregisterKeyBinding(name);
	}

	bool registerKeyListener(const std::string& name, const boost::shared_ptr<OIS::KeyListener>& listener)
	{
		return keyListenerManager.registerListener(name, listener);
	}

	bool unregisterKeyListener(const std::string& name)
	{
		return keyListenerManager.unregisterListener(name);
	}

	bool registerMouseListener(const std::string& name, const boost::shared_ptr<OIS::MouseListener>& listener)
	{
		return mouseListenerManager.registerListener(name, listener);
	}

	bool unregisterMouseListener(const std::string& name)
	{
		return mouseListenerManager.unregisterListener(name);
	}

} } }
