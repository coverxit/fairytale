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

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>
#include <utility>
#include <vector>

namespace OIS {

	enum KeyCode;
	class KeyListener;
	class MouseListener;
	class KeyEvent;

}

namespace fairytale { namespace engine {
	
	// Thread-safe class
	class InputManager
	{
	private:
		struct InputManagerImpl;
		InputManagerImpl* _mImpl;

	public:
		InputManager(size_t windowHandle);
		~InputManager();

		enum class KeyState
		{
			NONE = 0,
			UP = 1,
			DOWN = 2
		};

		struct KeyBinding
		{
			OIS::KeyCode keyCode;
			boost::uuids::uuid bindingId;
		};
		
		void setMouseBufferedArea(std::pair<int, int> widthAndHeight);

		boost::shared_ptr<KeyBinding> registerKeyBinding(OIS::KeyCode keyCode, KeyState state,
			const boost::function<void(const OIS::KeyEvent&)>& callback, const std::vector<OIS::KeyCode>& withNonCharKeys = std::vector<OIS::KeyCode>());
		bool unregisterKeyBinding(const boost::shared_ptr<KeyBinding>& bindingInfo);

		bool registerKeyListener(const boost::shared_ptr<OIS::KeyListener>& listener);
		bool unregisterKeyListener(const boost::shared_ptr<OIS::MouseListener>& listener);

		bool registerMouseListener(const boost::shared_ptr<OIS::MouseListener>& listener);
		bool unregisterMouseListener(const boost::shared_ptr<OIS::MouseListener>& listener);

		void processOneInputFrame();
	};

} }

#endif
