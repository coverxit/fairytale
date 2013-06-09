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

#ifndef __FAIRYTALE_LOGIC_EVENT_EVENT_HPP__
#define __FAIRYTALE_LOGIC_EVENT_EVENT_HPP__

#include <functional>
#include <string>
#include <memory>
#include <vector>

namespace fairytale {

    class Event {
    protected:
        std::string m_eventName;
        std::vector<std::unique_ptr<void>> m_paramStack;

        template<typename T>
        void _pushParam(T&& arg)
        {
            m_paramStack.push_back(std::unique_ptr<void>(new T(std::forward<T>(arg))));
        }

        template<typename Head, typename...Tail>
        void _pushParam(Head&& head, Tail&&...tail)
        {
            _pushParam(std::forward<Head>(head));
            _pushParam(std::forward<Tail>(tail)...);
        }

        template<typename T>
        T _popParam(size_t& paramIndex)
        {
            // vectorIndex + paramIndex(stackIndex) = vector.size() - 1
            size_t vectorIndex = m_paramStack.size() - 1 - paramIndex;
            ++paramIndex;
            return T(*(static_cast<T*>(m_paramStack[vectorIndex].get())));
        }

    public:
        Event(std::string&& eventName)
            : m_eventName(std::forward<std::string>(eventName)) {}

        virtual ~Event() {}

        std::string getEventName()
        {
            return m_eventName;
        }
    };

    class EventHandler {
    public:
        virtual void handleEvent(Event* evt) = 0;
        virtual ~EventHandler() {}
    };

    template<typename...Args> class _EventHandlerImpl;

    template<typename...Args>
    class _EventImpl : public Event {
        friend class _EventHandlerImpl<Args...>;

    public:
        _EventImpl(std::string&& eventName, Args&&...args)
            : Event(std::forward<std::string>(eventName))
        {
            _pushParam(std::forward<Args>(args)...);
        }

        virtual ~_EventImpl() {}
    };

    template<typename...Args>
    class _EventHandlerImpl : public EventHandler {
    public:
        typedef std::function<void(Event*, Args...)> CallbackType;

    protected:
        CallbackType m_callback;

    public:
        _EventHandlerImpl(CallbackType&& callback) : m_callback(std::forward<CallbackType>(callback)) {}

        virtual void handleEvent(Event* evt)
        {
            _EventImpl<Args...>* realEvent = static_cast<_EventImpl<Args...>*>(evt);
            size_t stackIndex(0);
            m_callback(evt, std::forward<Args>(realEvent->template _popParam<Args>(stackIndex))...);
        }

        virtual ~_EventHandlerImpl() {}
    };

    // Factory function. Returns naked pointer.
    template<typename...Args>
    Event* makeEvent(std::string eventName, Args...args)
    {
        return new _EventImpl<Args...>(std::move(eventName), std::move(args)...);
    }

    // Factory function. Returns naked pointer.
    template<typename...Args>
    EventHandler* makeEventHandler(typename _EventHandlerImpl<Args...>::CallbackType callback)
    {
        return new _EventHandlerImpl<Args...>(std::move(callback));
    }

}

#endif /* End of __FAIRYTALE_LOGIC_EVENT_EVENT_HPP__ */
