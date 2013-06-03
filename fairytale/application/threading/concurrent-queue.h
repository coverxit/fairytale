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

#ifndef __FAIRYTALE_APPLICATION_THREADING_CONCURRENT_QUEUE_H__
#define __FAIRYTALE_APPLICATION_THREADING_CONCURRENT_QUEUE_H__

#include <queue>
#include <boost/thread.hpp>
#include <functional>

namespace fairytale {

	template<typename T>
	class ConcurrentQueue
	{
	private:
		std::queue<T> _mDataQueue;
		mutable boost::mutex _mutex;
		boost::condition_variable _mQueueCondition;

	public:
		void push(const Data& data)
		{
			boost::mutex::scoped_lock lock(_mutex);
			_mDataQueue.push(data);
			lock.unlock();
			_mQueueCondition.notify_one();
		}

		void waitAndProcess(std::function<void(Data&)> processorFunctor)
		{
			boost::mutex::scoped_lock lock(_mutex);
			while(_mDataQueue.empty())
			{
				_mQueueCondition.wait(lock);
			}

			processorFunctor(_mDataQueue.front());
			_mDataQueue.pop();
		}

	};

}

#endif
