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

#define __FAIRYTALE_UNITTEST__

#include "unittest/unittest.hpp"
#include "logic/event/event.hpp"
#include <vector>
#include <thread>
#include <iostream>

using namespace fairytale;
using namespace std;

typedef SpecifiedEvent<int, std::string, std::string> speedTestEvent;
typedef SpecifiedEvent<int, fairytale::TestClass, std::string> valueTestEvent;

void callback(Event* evt, int num, fairytale::TestClass, std::string str)
{
    cout << evt->getEventName() << endl;
    cout << num << endl;
    cout << str << endl;
}

void callback2(Event* evt, int num, std::string str, std::string str2)
{}

void stressTest()
{
    fairytale::AutoTimer t;
    for(unsigned i = 0; i < 10000; ++i)
    {
        std::unique_ptr<Event> evt(new speedTestEvent("speedTest", 4096, "string1", "string2"));
        std::unique_ptr<EventHandler> handler(new speedTestEvent::Handler(&callback2));
        for(unsigned n = 0; n < 100; ++n)
            handler->handleEvent(evt.get());
    }
}

int main()
{

    {
        fairytale::TestClass anotherEle;
        std::unique_ptr<Event> evt(new valueTestEvent("lvalueTest", 1024, anotherEle, "lvalueTestStr"));
        std::unique_ptr<EventHandler> handler(new valueTestEvent::Handler(&callback));
        handler->handleEvent(evt.get());
    }

    {
        std::unique_ptr<Event> evt(new valueTestEvent("rvalueTest", 2048, fairytale::TestClass(), "rvalueTestStr"));
        //std::unique_ptr<EventHandler> handler(makeEventHandler<int, fairytale::TestClass, std::string>(&callback));
        std::unique_ptr<EventHandler> handler(new valueTestEvent::Handler(&callback));
        handler->handleEvent(evt.get());
    }

    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i)
    {
        workers.push_back(std::thread([]() { for(auto i = 0; i < 8; ++i) stressTest(); }));
    }
    for (auto& worker : workers) {
        worker.join();
    }

    stressTest();

    return 0;
}
