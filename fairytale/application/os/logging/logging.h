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

#ifndef __FAIRYTALE_OS_LOGGING_H__
#define __FAIRYTALE_OS_LOGGING_H__

#ifdef __FAIRYTALE_UNITTEST__

#include <iostream>

#define LOG(x) std::cout << "\nFunction: " << __FUNCTION__ << "(" << __LINE__ << "): "

#else /* __FAIRYTALE_UNITTEST__ */

#include <g2log/g2log.h>
#include <g2log/g2logworker.h>
#include <boost/serialization/singleton.hpp>

namespace fairytale {

	// boost::singleton::singleton will init the instance before entering main() so I use it here.
	class LogManager : public boost::serialization::singleton<LogManager> {
	private:
		g2LogWorker _mLogger;
		
	protected:
		LogManager();
		~LogManager();
	};

}

#endif /* End of __FAIRYTALE_UNITTEST__ */

#endif /* End of __FAIRYTALE_OS_LOGGING_H__ */
