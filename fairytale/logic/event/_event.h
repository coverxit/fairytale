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

#ifndef __FAIRYTALE_LOGIC_EVENT_EVENT_H__
#define __FAIRYTALE_LOGIC_EVENT_EVENT_H__

#include <functional>
#include <boost/thread.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <atomic>

#include "util/singleton.h"
#include "logic/interpreter/chaiscript.h"
#include "logic/structure/element.h"
#include "application/threading/concurrent-queue.h"
#include "application/threading/thread-pool.h"

namespace fairytale {

	// ************************************ EVENT ************************************

	class EventBase {
	protected:
		std::string _mEventName;
		Element* _mRootElement;
		std::atomic_bool _mCancelled;
		std::vector<chaiscript::Boxed_Value> _mParams;

	public:
		// Handler's first param should be current event's handle.
		EventBase(const std::string& eventName, Element* rootElement) : _mEventName(eventName), _mRootElement(rootElement)
		{
			_mParams.push_back(chaiscript::Boxed_Value(this));
		}

		virtual ~EventBase() {}

		void setCancelled(bool cancelled)
		{
			_mCancelled = cancelled;
		}

		bool isCancelled()
		{
			return _mCancelled;
		}

		void executeHandler(const chaiscript::Proxy_Function& handler)
		{
			(*handler)(_mParams);
		}

		std::string getEventName()
		{
			return _mEventName;
		}

		Element* getRootElement()
		{
			return _mRootElement;
		}
	};

	template<typename...Args>
	class SpecifiedEvent : public EventBase {
	public:
		SpecifiedEvent(const std::string& eventName, Element* rootElement, Args...args) : EventBase(eventName, rootElement)
		{
			_mParams.push_back(chaiscript::Boxed_Value(args...));
		}

		virtual ~SpecifiedEvent() {}
	};

	// this is a wrapper of EventBase*
	typedef std::shared_ptr<EventBase> Event;

	// ************************************ EVENT HANDLER ************************************
	
	struct EventHandlerBase {
		std::string eventName;
		Element* rootElement;
		chaiscript::Proxy_Function handler;
		// Will this handler process events propagated from childs.
		bool receivePropagated;

		EventHandlerBase(const std::string& eventName, Element* rootElement, const chaiscript::Proxy_Function& handler, bool receivePropagated)
			: eventName(eventName), rootElement(rootElement), handler(handler), receivePropagated(receivePropagated) {}
		virtual ~EventHandlerBase() {}
	};

	struct InvalidEventHandler : public EventHandlerBase {
		InvalidEventHandler() : EventHandlerBase("", nullptr, chaiscript::Proxy_Function(), false) {}
	};

	// Pointer wrapper.
	typedef std::shared_ptr<EventHandlerBase> EventHandler;

	// ************************************ EVENT MANAGER ************************************

	class EventManager : public Singleton<EventManager> {
	protected:
		// <EventName, <RootElement, <EventHandler*[ID], EventHandler>>>
		std::unordered_map<std::string, std::unordered_map<Element*, std::unordered_map<EventHandlerBase*, EventHandler>>> _mHandlerMap;
		boost::shared_mutex _mMapMutex;
		ConcurrentQueue<Event> _mEventQueue;
		std::thread* _mEventProcessThread;

		EventManager();
		~EventManager();

	public:
		// return false if existed.
		bool addEventCategory(const std::string& eventName);
		// May be harmful.
		//bool removeEventCategory(const std::string& eventName);

		EventHandler addEventHandler(const std::string& eventName, Element* rootElement, chaiscript::Proxy_Function handler, bool getPropagated);

		template<typename T>
		EventHandler addEventHandler(const std::string& eventName, Element* rootElement, const std::function<T>& handler, bool getPropagated)
		{
			return addEventHandler(eventName, chaiscript::Proxy_Function(new chaiscript::dispatch::Proxy_Function_Impl(handler)));
		}

		bool removeEventHandler(EventHandler handlerInfo);

		// return false if event was cancelled by one of bound handlers.
		template<typename...Args>
		bool triggerEvent(const std::string& eventName, Element* rootElement, Args...args)
		{
			/* static void* autoExport = ChaiScriptVM::getInstance().getVM()
				.add(fun(&EventManager::triggerEvent<Args>, this)); */

			_mEventQueue.push(Event(new SpecifiedEvent(eventName, rootElement, args)));
		}

		void processEvent(Event& evt);
		void clearBoundHandler(Element* rootElement);

	protected:
		void _doCallHandler();
	};

}

#endif
