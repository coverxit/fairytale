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

#include "thread-pool.h"
#include "application/os/logging/logging.h"

namespace fairytale {
	
	ThreadPtrHolder::ThreadPtrHolder(const std::shared_ptr<std::thread>& ptr) : threadPtr(ptr)
	{
		LOG(INFO) << "Added thread " << threadPtr.get() << " to pool.";
	}

	ThreadPtrHolder::~ThreadPtrHolder()
	{
		LOG(INFO) << "Removed thread " << threadPtr.get() << " from pool.";
	}

	ThreadPool::ThreadPool()
	{
		LOG(INFO) << "ThreadPool loaded.";
	}

	ThreadPool::~ThreadPool()
	{
		LOG(INFO) << "ThreadPool unloaded.";
	}

	std::thread* ThreadPool::add(std::thread* newThread)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto inserted = _mPool.insert(std::make_pair(newThread, ThreadPtrHolder(std::shared_ptr<std::thread>(newThread))));
		if(inserted.second)
		{
			return newThread;
		} else {
			LOG(INFO) << "Thread " << newThread << " has already been in pool.";
			return nullptr;
		}
	}

	bool ThreadPool::remove(std::thread* threadPtr)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if(_mPool.erase(threadPtr))
		{
			return true;
		} else {
			LOG(INFO) << "Thread " << threadPtr << " is not in pool.";
			return false;
		}
	}

}
