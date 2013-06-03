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

#include "mouse.h"
#include "application/os/logging/logging.h"

namespace fairytale {

	bool MouseListenerManager::mouseMoved(const MouseEvent &arg)
	{
		_mMouseMoveCallback(arg);
	}

	bool MouseListenerManager::mousePressed(const MouseEvent &arg, MouseButtonID id)
	{
		auto iter = _mMousePressBinding.find(id);
		if(iter != _mMousePressBinding.end())
		{
			iter->second(arg);
		}
	}

	bool MouseListenerManager::mouseReleased(const MouseEvent &arg, MouseButtonID id)
	{
		auto iter = _mMouseReleaseBinding.find(id);
		if(iter != _mMouseReleaseBinding.end())
		{
			iter->second(arg);
		}
	}

	bool MouseListenerManager::_doBind(MouseBindingMap& map, MouseButtonID btnId, const MouseEventCallback& cb)
	{
		map[btnId] = cb;
	}

	bool MouseListenerManager::_doUnbind(MouseBindingMap& map, MouseButtonID btnId)
	{
		auto iter = map.find(btnId);
		if(iter != map.end())
		{
			return false;
		} else {
			map.erase(iter);
			return true;
		}
	}

	void MouseListenerManager::bindMouseButtonPressed(MouseButtonID btnId, const MouseEventCallback& cb)
	{
		std::lock_guard<std::mutex> lock(_mPressMutex);
		if(_doBind(_mMousePressBinding, btnId, cb))
		{
			LOG(WARNING) << "Last binding with mouse button " << btnId << ".onPressed was lost."; 
		}
		LOG(INFO) << "Mouse button " << btnId << ".onPressed is bound.";
	}

	void MouseListenerManager::unbindMouseButtonPressed(MouseButtonID btnId)
	{
		std::lock_guard<std::mutex> lock(_mPressMutex);
		if(_doUnbind(_mMousePressBinding, btnId))
		{
			LOG(INFO) << "Mouse button " << btnId << ".onPressed is now unbound.";
		} else {
			LOG(WARNING) << "Mouse button " << btnId << ".onPressed is not bound.";
		}
	}

	void MouseListenerManager::bindMouseButtonReleased(MouseButtonID btnId, const MouseEventCallback& cb)
	{
		std::lock_guard<std::mutex> lock(_mReleaseMutex);
		if(_doBind(_mMouseReleaseBinding, btnId, cb))
		{
			LOG(WARNING) << "Last binding with mouse button " << btnId << ".onReleased was lost."; 
		}
		LOG(INFO) << "Mouse button " << btnId << ".onReleased is bound.";
	}

	void MouseListenerManager::unbindMouseButtonReleased(MouseButtonID btnId)
	{
		std::lock_guard<std::mutex> lock(_mReleaseMutex);
		if(_doUnbind(_mMouseReleaseBinding, btnId))
		{
			LOG(INFO) << "Mouse button " << btnId << ".onReleased is now unbound.";
		} else {
			LOG(WARNING) << "Mouse button " << btnId << ".onReleased is not bound.";
		}
	}

	void MouseListenerManager::bindMouseMoved(const MouseEventCallback& cb)
	{
		_mMouseMoveCallback = cb;
		LOG(INFO) << "New mouse movement listener registered.";
	}

	MouseListenerManager::MouseListenerManager()
	{
		LOG(INFO) << "MouseListenerManager loaded.";
	}

	MouseListenerManager::~MouseListenerManager()
	{
		LOG(INFO) << "MouseListenerManager unloaded.";
	}

}
