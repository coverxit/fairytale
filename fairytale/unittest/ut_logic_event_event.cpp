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

#include "unittest/unittest.h"
#include "logic/event/event.h"
#include <vector>
#include <thread>
 
namespace fairytale {

class Element
{
public:
    // Simple constructor
    Element() { std::cout << "Element::Simple constructor" << std::endl; }
    // Destructor
    virtual ~Element() { std::cout << "Element::Destructor" << std::endl; }

    // Copy constructor
    Element(const Element& rv) { std::cout << "Element::Copy constructor" << std::endl; }
    // Copy assignment operator
    Element& operator=(const Element& other) { std::cout << "Element::Copy assignment operator" << std::endl; return *this; }

    // Move constructor
    Element(Element&& rvref) { std::cout << "Element::Move constructor" << std::endl; }
    // Move assignment operator
    Element& operator=(Element&& other) { std::cout << "Element::Move assignment operator" << std::endl; return *this; }

};

}

void callback(const fairytale::SharedEventPtr& evt, int num, fairytale::TestClass, std::string str)
{
    LOG(INFO) << evt->getEventName();
    LOG(INFO) << num;
    LOG(INFO) << str;
}

void callback2(const fairytale::SharedEventPtr& evt, int num, std::string str, std::string str2)
{}

void stressTest()
{
    typedef fairytale::Event<int, std::string, std::string> BarEvent;
    fairytale::AutoTimer t;
    auto ele = std::make_shared<fairytale::Element>();
    for(unsigned i = 0; i < 10000; ++i)
    {
        auto evt = std::make_shared<BarEvent>("speedTest", ele, 4096, "string1", "string2");
        auto handler = std::make_shared<BarEvent::Handler>(&callback2);
        for(unsigned n = 0; n < 100; ++n)
            handler->handleEvent(evt);
    }
}

int main()
{
    typedef fairytale::Event<int, fairytale::TestClass, std::string> FooEvent;

    {
        auto ele = std::make_shared<fairytale::Element>();
        fairytale::TestClass anotherEle;
        auto evt = std::make_shared<FooEvent>("lvalueTest", ele, 1024, anotherEle, "lvalueTestStr");
        auto handler = std::make_shared<FooEvent::Handler>(&callback);
        handler->handleEvent(evt);
    }

    {
        auto ele = std::make_shared<fairytale::Element>();
        auto evt = std::make_shared<FooEvent>("rvalueTest", ele, 2048, fairytale::TestClass(), "rvalueTestStr");
        auto handler = std::make_shared<FooEvent::Handler>(&callback);
        handler->handleEvent(evt);
    }

    std::vector<std::thread> workers;
    for (int i = 0; i < 8; ++i)
    {
        workers.push_back(std::thread(&stressTest));
    }
    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}
