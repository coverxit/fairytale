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

#include <deque>

namespace fairytale
{
	class KeyListenerManager : public Singleton<KeyListenerManager>, OIS::KeyListener
	{
	public:
		enum OpreateType
		{
			KEY_UP = 0,
			KEY_DOWN = 1
		};

		typedef boost::function<void(const OIS::KeyEvent&)> KeyListener;

		void registerListener(OpreateType type, OIS::KeyCode key, const KeyListener& listener);

		bool keyPressed(const OIS::KeyEvent& evt);
		bool keyReleased(const OIS::KeyEvent& evt);

	private:
		typedef std::deque<KeyListener> ListenerSet;
		typedef std::unordered_map<OIS::KeyCode, ListenerSet> ListenerMap;

		ListenerMap _uplisteners;
		ListenerMap _downlisteners;
	};

	class MouseListenerManager : public Singleton<MouseListenerManager>, OIS::MouseListener
	{
	public:
		enum OpreateType
		{
			MOUSE_UP = 0,
			MOUSE_DOWN = 1
		};

		typedef boost::function<void(const OIS::MouseEvent&)> MouseMoveListener;
		typedef boost::function<void(const OIS::MouseEvent&, OIS::MouseButtonID)> MouseClickListener;

		void registerMoveListener(const MouseMoveListener& listener);
		void registerClickListener(OpreateType type, OIS::MouseButtonID btn, const MouseClickListener& listener);

		bool mouseMoved(const OIS::MouseEvent& evt);
		bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	private:
		typedef std::deque<MouseMoveListener> MoveListenerSet;
		typedef std::deque<MouseClickListener> ClickListenerSet;
		typedef std::unordered_map<OIS::MouseButtonID, ClickListenerSet> ClickListenerMap;
		
		MoveListenerSet _movelisteners;
		ClickListenerMap _uplisteners;
		ClickListenerMap _downlisteners;
	};
}

#endif
