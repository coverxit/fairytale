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

#include "pch.h"
#include "application.h"

namespace fairytale
{
	void Application::InitEngine(
		const std::string& pluginsfilename,
		const std::string& configfilename,
		const std::string& logfilename)
	{
		// Create Ogre::Root object with specified config files.
		// I think plugins will be loaded automatically...
		root_.reset(new Ogre::Root(
			pluginsfilename, configfilename, logfilename));

		// I 
		if(root_->showConfigDialog())
		{
			window_.reset(root_->initialise(true, "fairytale"));
		} else {
			throw std::runtime_error("Couldn't create config dialog.");
		}
	}

	void Application::StartMessagePump()
	{
		while(!window_->isClosed())
		{
			Ogre::WindowEventUtilities::messagePump();
		}
	}
};
