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

#ifndef __FAIRYTALE_APP_DEV_INPUT_OIS_H__
#define __FAIRYTALE_APP_DEV_INPUT_OIS_H__

#include "pch.h"
#include "util/singleton.h"

namespace fairytale {

	class OISInstance : public Singleton<OISInstance> {
	protected:
		OIS::InputManager* _mInputMgr;
		OIS::Keyboard* _mKeyboard;
		OIS::Mouse* _mMouse;

		OISInstance();
		~OISInstance();

	public:
		void init(size_t windowHandle);
	};

}

#endif
