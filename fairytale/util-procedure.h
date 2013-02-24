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

#ifndef __FAIRYTALE_UTIL_PROCEDURE_H__
#define __FAIRYTALE_UTIL_PROCEDURE_H__

#include <iostream>

#define ONE_OFF_FUNCTION(returntype, value) do { static bool __called = false; if(__called) { std::cout << "Warning: Function " << __FUNCTION__ << " is a one-off function." << std::endl; return returntype(value); } else __called = true; } while(false)
#define ONE_OFF_FUNCTION_VOID ONE_OFF_FUNCTION(void,)

#endif
