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

#include "engine-pch.h"
#include "engine-input.h"
#include "engine-application.h"

namespace fairytale
{
	void KeyListenerManager::registerListener(KeyListenerManager::OpreateType type, OIS::KeyCode key, const KeyListenerManager::KeyListener& listener)
	{
		switch(type)
		{
		case KEY_UP:
			_uplisteners[key].push_back(listener);
			break;

		case KEY_DOWN:
			_downlisteners[key].push_back(listener);
			break;
		}
	}

	bool KeyListenerManager::keyPressed(const OIS::KeyEvent& evt)
	{
		Application::getInstance().mCameraMan->injectKeyDown(evt);

		ListenerSet& set = _downlisteners[evt.key];

		for(ListenerSet::iterator iter(set.begin()); iter != set.end(); ++iter)
		{
			(*iter)(evt);
		}

		return true;
	}

	bool KeyListenerManager::keyReleased(const OIS::KeyEvent& evt)
	{
		Application::getInstance().mCameraMan->injectKeyUp(evt);

		ListenerSet& set = _uplisteners[evt.key];

		for(ListenerSet::iterator iter(set.begin()); iter != set.end(); ++iter)
		{
			(*iter)(evt);
		}

		return true;

	}

	void MouseListenerManager::registerMoveListener(const MouseMoveListener& listener)
	{
		_movelisteners.push_back(listener);
	}

	void MouseListenerManager::registerClickListener(OpreateType type, OIS::MouseButtonID btn, const MouseClickListener& listener)
	{
		switch(type)
		{
		case MOUSE_UP:
			_uplisteners[btn].push_back(listener);
			break;

		case MOUSE_DOWN:
			_downlisteners[btn].push_back(listener);
			break;
		}
	}

	bool MouseListenerManager::mouseMoved(const OIS::MouseEvent& evt)
	{
		Application::getInstance().mCameraMan->injectMouseMove(evt);

		for(MoveListenerSet::iterator iter(_movelisteners.begin()); iter != _movelisteners.end(); ++iter)
		{
			(*iter)(evt);
		}

		return true;
	}

	bool MouseListenerManager::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		Application::getInstance().mCameraMan->injectMouseDown(evt, id);

		ClickListenerSet& set = _downlisteners[id];

		for(ClickListenerSet::iterator iter(set.begin()); iter != set.end(); ++iter)
		{
			(*iter)(evt, id);
		}

		return true;
	}

	bool MouseListenerManager::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		Application::getInstance().mCameraMan->injectMouseUp(evt, id);

		ClickListenerSet& set = _downlisteners[id];

		for(ClickListenerSet::iterator iter(set.begin()); iter != set.end(); ++iter)
		{
			(*iter)(evt, id);
		}

		return true;
	}
}
