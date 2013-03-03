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

#ifndef __FAIRYTALE_ENGINE_INPUT_H__
#define __FAIRYTALE_ENGINE_INPUT_H__

#include "pch.h"

namespace fairytale { namespace engine { namespace input {

	void initOIS(size_t hWindow);

	OIS::Keyboard* getKeyboard();
	OIS::Mouse* getMouse();

	void setMouseBufferedArea(int width, int height);

	enum KeyState
	{
		NONE = 0,
		UP = 1,
		DOWN = 2
	};

	bool registerKeyBinding(const std::string& name, OIS::KeyCode keycode, KeyState state, const boost::function<void(const OIS::KeyEvent&)>& action);
	bool unregisterKeyBinding(const std::string& name);

	bool registerKeyListener(const std::string& name, const boost::shared_ptr<OIS::KeyListener>& listener);
	bool unregisterKeyListener(const std::string& name);

	bool registerMouseListener(const std::string& name, const boost::shared_ptr<OIS::MouseListener>& listener);
	bool unregisterMouseListener(const std::string& name);

} } }

#endif
