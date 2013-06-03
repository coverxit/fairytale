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

#include <string>

#include "ois.h"
#include "application/os/logging/logging.h"
#include "mouse.h"
#include "keyboard.h"

namespace fairytale {

	OISInstance::OISInstance() : _mInputMgr(nullptr), _mKeyboard(nullptr), _mMouse(nullptr)
	{
		LOG(WARNING) << "OISIntance's constructor is called, but it's required to call init(windowHandle) to init OIS actually.";
	}

	OISInstance::~OISInstance()
	{
		if(_mMouse) _mInputMgr->destroyInputObject(_mMouse);
		if(_mKeyboard) _mInputMgr->destroyInputObject(_mKeyboard);
		if(_mInputMgr) OIS::InputManager::destroyInputSystem(_mInputMgr);
		LOG(INFO) << "OISInstance unloaded.";
	}

	void OISInstance::init(size_t windowHandle)
	{
		OIS::ParamList paramlist;
		paramlist.insert(OIS::ParamList::value_type("WINDOW", std::to_string(windowHandle)));

		_mInputMgr = OIS::InputManager::createInputSystem(paramlist);
		_mKeyboard = static_cast<OIS::Keyboard*>(_mInputMgr->createInputObject(OIS::OISKeyboard, true));
		_mMouse = static_cast<OIS::Mouse*>(_mInputMgr->createInputObject(OIS::OISMouse, true));

		_mKeyboard->setEventCallback(KeyListenerManager::getInstancePtr());
		_mMouse->setEventCallback(MouseListenerManager::getInstancePtr());

		LOG(INFO) << "OIS inited.";
	}

}
