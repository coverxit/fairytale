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

#ifndef __FAIRYTALE_APPLICATION_H__
#define __FAIRYTALE_APPLICATION_H__

#include <string>
#include <boost/shared_ptr.hpp>

namespace Ogre
{
	class Root;
	class RenderWindow;
};

namespace fairytale
{
	class Application
	{
		boost::shared_ptr<Ogre::Root> root_;
		boost::shared_ptr<Ogre::RenderWindow> window_;

	public:
		void InitEngine(
			const std::string& pluginsfilename,
			const std::string& configfilename,
			const std::string& logfilename);
		void StartMessagePump();
	};
};

#endif
