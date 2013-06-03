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
#include <string>
#include <memory>
#include <atomic>
#include <vector>

#include "application/os/logging/logging.h"

namespace fairytale {

    class Element;

    typedef std::shared_ptr<class EventBase> SharedEventPtr;

    class EventBase {
    protected:
        std::string _mEventName;
        std::shared_ptr<Element> _mTriggerElement;
        std::atomic_bool _mCancelled;
        
    public:
        EventBase(std::string eventName, std::shared_ptr<Element> triggerElement)
            : _mEventName(std::move(eventName)), _mTriggerElement(std::move(triggerElement)) {}

        virtual ~EventBase() = 0;

        void setCancelled(bool cancelled)
        {
            _mCancelled = cancelled;
        }

        bool isCancelled()
        {
            return _mCancelled;
        }

        std::string getEventName()
        {
            return _mEventName;
        }

        std::shared_ptr<Element> getTriggerElement()
        {
            return _mTriggerElement;
        }

        class HandlerBase {
        public:
            virtual void handleEvent(const SharedEventPtr& evt) = 0;
            virtual ~HandlerBase() = 0;
        };
    };

    template<typename...Args>
    class Event : public EventBase {
        friend class Handler;
        typedef std::vector<std::shared_ptr<void>> ParamStack;

    protected:
        ParamStack _mParams;

        template<typename T>
        void _pushParam(T&& arg)
        {
            _mParams.push_back(std::move(std::shared_ptr<void>(new T(std::forward<T>(arg)))));
        }

        template<typename Head, typename...Tail>
        void _pushParam(Head&& head, Tail&&...tail)
        {
            _pushParam(std::forward<Head>(head));
            _pushParam(std::forward<Tail>(tail)...);
        }

        template<typename T>
        T _popParam(std::atomic<size_t>& paramIndex)
        {
        	// vectorIndex + paramIndex(stackIndex) = vector.size() - 1
        	size_t vectorIndex = _mParams.size() - 1 - paramIndex;
        	++paramIndex;
            return T(std::move(*(static_cast<T*>(_mParams[vectorIndex].get()))));
        }

    public:
        Event(std::string eventName, std::shared_ptr<Element> triggerElement, Args...args)
            : EventBase(std::move(eventName), std::move(triggerElement))
        {
            _pushParam(std::move(args)...);
        }

        virtual ~Event() {}

        class Handler : public EventBase::HandlerBase {
        public:
            typedef std::function<void(const SharedEventPtr&, Args...)> CallbackType;

        protected:
            CallbackType _mCallback;

        public:
            Handler(CallbackType callback) : _mCallback(std::move(callback)) {}

            virtual void handleEvent(const SharedEventPtr& evt)
            {
                Event<Args...>* realEvent = dynamic_cast<Event<Args...>*>(evt.get());
                if(!realEvent)
                {
                    LOG(WARNING) << "Event doesn't match this handler (dynamic_cast failed). Event is ignored.";
                    return;
                }
                std::atomic<size_t> stackIndex(0);
                _mCallback(evt, std::move(realEvent->template _popParam<Args>(stackIndex))...);
            }

            virtual ~Handler() {}
        };
    };

    EventBase::~EventBase() {}
    EventBase::HandlerBase::~HandlerBase() {}

}

#endif /* End of __FAIRYTALE_LOGIC_EVENT_EVENT_H__ */
