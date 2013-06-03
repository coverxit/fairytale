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

#ifndef __FAIRYTALE_APP_DEV_INPUT_KEYBOARD_H__
#define __FAIRYTALE_APP_DEV_INPUT_KEYBOARD_H__

#include <unordered_map>
#include <functional>
#include <mutex>

#include "pch.h"
#include "util/singleton.h"

namespace fairytale {

	typedef OIS::KeyEvent KeyEvent;
	typedef OIS::KeyCode KeyCode;
	typedef std::function<void(const KeyEvent&)> KeyEventCallback;

	class KeyListenerManager : public Singleton<KeyListenerManager>, public OIS::KeyListener
	{
	protected:
		typedef std::unordered_map<KeyCode, KeyEventCallback> KeyBindingMap;
		KeyBindingMap _mKeyPressBinding, _mKeyReleaseBinding;
		std::mutex _mPressMutex, _mReleaseMutex;

		// return true if rewriting was required
		bool _doBind(KeyBindingMap& map, KeyCode btnId, const MouseEventCallback& cb);

		// return true if binding is removed(existed)
		bool _doUnbind(KeyBindingMap& map, MouseButtonID btnId);

		KeyListenerManager();
		~KeyListenerManager();

	public:
		bool keyPressed(const KeyEvent& evt);
		bool keyReleased(const KeyEvent& evt);

		void bindMouseButtonPressed(MouseButtonID btnId, const MouseEventCallback& cb);
		void unbindMouseButtonPressed(MouseButtonID btnId);
		void bindMouseButtonReleased(MouseButtonID btnId, const MouseEventCallback& cb);
		void unbindMouseButtonReleased(MouseButtonID btnId);
		void bindMouseMoved(const MouseEventCallback& cb);
	};

}

#endif
