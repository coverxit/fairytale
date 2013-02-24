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

#ifndef __FAIRYTALE_UTIL_SINGLETON_H__
#define __FAIRYTALE_UTIL_SINGLETON_H__

#include "engine-pch.h"

namespace fairytale
{
	template<class T>
	// This implementation is NOT Thread-Safe. But here i have a mutex lock for you.
	class Singleton : private boost::noncopyable
	{
	public:
		static T& getInstance()
		{
			return *getInstancePtr();
		}

		static T* getInstancePtr()
		{
			static boost::scoped_ptr<T> _instance(new T());
			return _instance.get();
		}

		boost::mutex _mutex;
	};
}

#define LOCK_AND_GET_INSTANCE_PTR(typename, ptrname) boost::mutex::scoped_try_lock __##typename_lock_##ptrname(typename::getInstancePtr()->_mutex); typename* ptrname = typename::getInstancePtr();

#endif
