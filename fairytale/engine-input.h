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

#ifndef __FAIRYTALE_ENGINE_INPUT_H__
#define __FAIRYTALE_ENGINE_INPUT_H__

#include "engine-pch.h"
#include "util-singleton.h"

namespace fairytale
{
	class KeyListenerManager : public Singleton<KeyListenerManager>, public OIS::KeyListener
	{
	public:
		enum KeyState
		{
			UP,
			DOWN,
			BOTH
		};

		typedef boost::function<void(const OIS::KeyEvent&)> KeyHandler;

		void bindKey(OIS::KeyCode keycode, KeyState state, const KeyHandler& action);
		void unbindKey(OIS::KeyCode keycode, KeyState state);

		void registerListener(const std::string& name, boost::shared_ptr<OIS::KeyListener> listener);
		void unregisterListener(const std::string& name);

		bool keyPressed(const OIS::KeyEvent& evt);
		bool keyReleased(const OIS::KeyEvent& evt);

	private:
		typedef std::unordered_map<OIS::KeyCode, KeyHandler> KeyActionMap;
		typedef std::unordered_map<std::string, boost::shared_ptr<OIS::KeyListener>> ListenerMap;
		
		ListenerMap _general;
		KeyActionMap _pressbinds;
		KeyActionMap _releasebinds;
	};

	class MouseListenerManager : public Singleton<MouseListenerManager>, public OIS::MouseListener
	{
	public:
		void registerListener(const std::string& name, boost::shared_ptr<OIS::MouseListener> listener);
		void unregisterListener(const std::string& name);

		bool mouseMoved(const OIS::MouseEvent& evt);
		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	private:
		typedef std::unordered_map<std::string, boost::shared_ptr<OIS::MouseListener>> ListenerMap;
		
		ListenerMap _general;
	};
}

#endif