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

#ifndef __FAIRYTALE_APPLICATION_THREADING_ACTIONS_H__
#define __FAIRYTALE_APPLICATION_THREADING_ACTIONS_H__

#include <queue>
#include <functional>
#include <memory>
#include <mutex>

namespace fairytale {

	typedef std::function<void()> Action;

	class PendingActionQueue {
	protected:
		std::shared_ptr<std::queue<Action>> _mMsgQueue;
		std::mutex _mQueueMutex;

	public:
		PendingActionQueue() : _mMsgQueue(new std::queue<Action>) {}

		void postAction(const Action& act)
		{
			std::lock_guard<std::mutex> lock(_mQueueMutex);
			_mMsgQueue->push(act);
		}

		// Swap out the queue filled with pending actions, then while(!empty()) { pop(); } or whatever.
		void swapOut(std::shared_ptr<std::queue<Action>>& dest)
		{
			std::lock_guard<std::mutex> lock(_mQueueMutex);
			_mMsgQueue.swap(dest);
			// We should always have a fresh queue
			if(!_mMsgQueue)
			{
				_mMsgQueue.reset(new std::queue<Action>);
			}
		}
	};

}

#endif
