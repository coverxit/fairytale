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

#ifndef __FAIRYTALE_APPLICATION_THREADING_THREADPOOL_H__
#define __FAIRYTALE_APPLICATION_THREADING_THREADPOOL_H__

#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>

#include "util/singleton.h"

namespace fairytale {
	
	struct ThreadPtrHolder {
		std::shared_ptr<std::thread> threadPtr;
		ThreadPtrHolder(const std::shared_ptr<std::thread>& ptr);
		~ThreadPtrHolder();
	};

	class ThreadPool : public Singleton<ThreadPool> {
	protected:
		std::unordered_map<std::thread*, ThreadPtrHolder> _mPool;
		std::mutex _mutex;

	public:
		ThreadPool();
		~ThreadPool();

		// pass a new-created thread here and it will be stored in shared_ptr automaticlly.
		std::thread* add(std::thread* newThread);
		bool remove(std::thread* threadPtr);
	};

}

#endif
