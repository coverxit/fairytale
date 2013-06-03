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

#include "application/os/logging/logging.h"

#include "event.h"

namespace fairytale {

	EventManager::EventManager() : _mId(0)
	{
		auto vm = ChaiScriptVM::getInstance().getVM();

		// Event
		vm->add(chaiscript::user_type<EventBase>() "Event");
		vm->add(chaiscript::fun(&EventBase::getEventName), "getEventName");
		vm->add(chaiscript::fun(&EventBase::isCancelled), "isCancelled");
		vm->add(chaiscript::fun(&EventBase::setCancelled), "setCancelled");

		// Handler
		vm->add(chaiscript::user_type<EventHandlerBase>() "EventHandler");

		// Manager
		vm->add(chaiscript::fun(&EventManager::addEventCategory, this), "addEventCategory");
		vm->add(chaiscript::fun<EventHandler(const std::string&, Element*, chaiscript::Proxy_Function, bool)>
			(&EventManager::addEventHandler, this), "addEventHandler");
		vm->add(chaiscript::fun(&EventManager::removeEventHandler, this), "removeEventHandler");
		vm->add(chaiscript::fun<bool(const std::string&, Element*)>
			(&EventManager::triggerEvent, this), "triggerEvent");
		vm->add(chaiscript::fun<bool(const std::string&, Element*, chaiscript::Boxed_Value)>
			(&EventManager::triggerEvent, this), "triggerEvent");
		vm->add(chaiscript::fun<bool(const std::string&, Element*, chaiscript::Boxed_Value, chaiscript::Boxed_Value)>
			(&EventManager::triggerEvent, this), "triggerEvent");
		vm->add(chaiscript::fun<bool(const std::string&, Element*, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value)>
			(&EventManager::triggerEvent, this), "triggerEvent");
		vm->add(chaiscript::fun<bool(const std::string&, Element*, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value)>
			(&EventManager::triggerEvent, this), "triggerEvent");
		vm->add(chaiscript::fun<bool(const std::string&, Element*, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value, chaiscript::Boxed_Value)>
			(&EventManager::triggerEvent, this), "triggerEvent");

		_mEventProcessThread = ThreadPool::getInstance().add(new std::thread(
			std::bind(&ConcurrentQueue<Event>::waitAndProcess, &_mEventQueue, std::bind(&EventManager::processEvent, this))));

		LOG(INFO) << "EventManager loaded.";
	}

	EventManager::~EventManager()
	{
		LOG(INFO) << "EventManager unloaded.";
	}

	bool EventManager::addEventCategory(const std::string& eventName)
	{
		boost::unique_lock<boost::shared_mutex> lock(_mMapMutex);
		return _mHandlerMap.insert(std::make_pair(eventName, std::unordered_map<Element*, std::unordered_map<EventHandlerBase*, EventHandler>>()).second;
	}

	EventHandler EventManager::addEventHandler(const std::string& eventName, Element* rootElement, chaiscript::Proxy_Function handler, bool getPropagated)
	{
		boost::unique_lock<boost::shared_mutex> lock(_mMapMutex);

		auto eventNameIter = _mHandlerMap.find(eventName);
		if(eventNameIter == _mHandlerMap.end())
		{
			LOG(WARNING) << "Event category \"" << eventName << "\" doen't exist.";
			return EventHandler(new InvalidEventHandler());
		}

		auto handler = EventHandler(new EventHandlerBase(eventName, rootElement, handler, getPropagated));
		eventNameIter->second[rootElement][*handler] = handler;

		LOG(INFO) << "Added event handler bound with " << eventName << " .";

		return handler;
	}

	bool EventManager::removeEventHandler(EventHandler handlerInfo)
	{
		boost::unique_lock<boost::shared_mutex> lock(_mMapMutex);

		// Find by EventName
		auto eventNameIter = _mHandlerMap.find(handlerInfo->eventName);
		if(eventNameIter != _mHandlerMap.end())
		{
			// Find by RootElement
			auto elementIter = eventNameIter->second.find(handlerInfo->rootElement);
			if(elementIter != eventNameIter->second.end())
			{
				// Find by EventHandler
				auto handlerIter = elementIter->second.find(*handlerInfo);
				if(handlerIter != elementIter->second.end())
				{
					auto r = elementIter->second.erase(handlerIter);
					if(r)
					{
						LOG(INFO) << "Event handler bound with " << handlerInfo->eventName << " (" << handlerInfo.get() << ") been removed.";
						return true;
					}
				}
			}
		}

		LOG(WARNING) << "Didn't find the event handler requested (" << handlerInfo.get() << ") removement bound with " << handlerInfo->eventName << ".";
		return false;
	}



	//todo
	void EventManager::processEvent(Event& evt)
	{
		boost::unique_lock<boost::shared_mutex> lock(_mMapMutex);

		// Find by EventName
		auto eventNameIter = _mHandlerMap.find(evt->getEventName());
		if(eventNameIter != _mHandlerMap.end())
		{
			// Find by RootElement
			auto elementIter = eventNameIter->second.find(handlerInfo->rootElement);
			if(elementIter != eventNameIter->second.end())
			{
				// Call RootElement's handler
				{
					for(auto handlerIter = elementIter->second.begin(); handlerIter != elementIter->second.end(); ++handlerIter)
					{
						evt.executeHandler(handlerIter->second->handler);
					}
				}
				// ????? call which handler????
				// Call RootElement's parent
				{
					Element* root = elementIter->first;
					Element* parent = root->getParent();
					while(parent != nullptr)
					{
						evt.executeHandler(handlerIter->second->handler);
						parent = root->getParent();
					}
				}
			}
		}
		// And element's parent & child
	}

}
