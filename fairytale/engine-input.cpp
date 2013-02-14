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
	void KeyListenerManager::bindKey(OIS::KeyCode keycode, KeyState state, const KeyHandler& action)
	{
		switch(state)
		{
		case BOTH:
		case DOWN:
			_pressbinds[keycode] = action;
			if(state == DOWN)
				break;

		case UP:
			_releasebinds[keycode] = action;
			break;
		}
	}

	void KeyListenerManager::unbindKey(OIS::KeyCode keycode, KeyState state)
	{
		switch(state)
		{
		case BOTH:
		case DOWN:
			_pressbinds.erase(keycode);
			if(state == DOWN)
				break;

		case UP:
			_releasebinds.erase(keycode);
			break;
		}
	}

	void KeyListenerManager::registerListener(const std::string& name, boost::shared_ptr<OIS::KeyListener> listener)
	{
		_general[name] = listener;
	}

	void KeyListenerManager::unregisterListener(const std::string& name)
	{
		_general.erase(name);
	}

	bool KeyListenerManager::keyPressed(const OIS::KeyEvent& evt)
	{
		KeyActionMap::iterator actioniter(_pressbinds.find(evt.key));
		if(actioniter != _pressbinds.end())
			actioniter->second(evt);

		for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
		{
			iter->second->keyPressed(evt);
		}

		return true;
	}

	bool KeyListenerManager::keyReleased(const OIS::KeyEvent& evt)
	{
		KeyActionMap::iterator actioniter(_releasebinds.find(evt.key));
		if(actioniter != _releasebinds.end())
			actioniter->second(evt);

		for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
		{
			iter->second->keyReleased(evt);
		}

		return true;

	}

	void MouseListenerManager::registerListener(const std::string& name, boost::shared_ptr<OIS::MouseListener> listener)
	{
		_general[name] = listener;

	}

	void MouseListenerManager::unregisterListener(const std::string& name)
	{
		_general.erase(name);
	}

	bool MouseListenerManager::mouseMoved(const OIS::MouseEvent& evt)
	{
		for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
		{
			iter->second->mouseMoved(evt);
		}

		return true;
	}

	bool MouseListenerManager::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
		{
			iter->second->mousePressed(evt, id);
		}

		return true;
	}

	bool MouseListenerManager::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		for(ListenerMap::iterator iter(_general.begin()); iter != _general.end(); ++iter)
		{
			iter->second->mouseReleased(evt, id);
		}

		return true;
	}
}
