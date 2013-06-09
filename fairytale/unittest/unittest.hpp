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

#ifndef __FAIRYTALE_UNITTEST_H__
#define __FAIRYTALE_UNITTEST_H__

#ifndef __FAIRYTALE_UNITTEST__
#define __FAIRYTALE_UNITTEST__
#endif /* End of __FAIRYTALE_UNITTEST__ */
 
#include <iostream>
#include <typeinfo>
#include <chrono>

namespace fairytale {

class TestClass
{
public:
    // Simple constructor
    TestClass() { std::cout << "TestClass::Simple constructor" << std::endl; }
    // Destructor
    virtual ~TestClass() { std::cout << "TestClass::Destructor" << std::endl; }

    // Copy constructor
    TestClass(const TestClass& rv) { std::cout << "TestClass::Copy constructor" << std::endl; }
    // Copy assignment operator
    TestClass& operator=(const TestClass& other) { std::cout << "TestClass::Copy assignment operator" << std::endl; return *this; }

    // Move constructor
    TestClass(TestClass&& rvref) { std::cout << "TestClass::Move constructor" << std::endl; }
    // Move assignment operator
    TestClass& operator=(TestClass&& other) { std::cout << "TestClass::Move assignment operator" << std::endl; return *this; }

};

class AutoTimer
{
protected:
    std::chrono::high_resolution_clock::time_point _mStart;

public:
    AutoTimer() : _mStart(std::chrono::high_resolution_clock::now()) {}

    ~AutoTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - _mStart).count()
              << " milliseconds." << std::endl;
    }
};

}

#endif /* End of __FAIRYTALE_UNITTEST_H__ */
