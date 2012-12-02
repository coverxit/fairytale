/*
 * Copyright 2012 TennenColl
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

#include "application.h"

using namespace boost::python;
using namespace fairytale;

BOOST_PYTHON_MODULE(fairytale) {
	class_<Application>("Application")
		.def("InitEngine",			&Application::InitEngine)
		.def("StartMessagePump",	&Application::StartMessagePump)
		;
}

int main()
{
	Py_Initialize();
	initfairytale();
#if OGRE_DEBUG_MODE == 1
	PyRun_SimpleString("execfile('./scripts/fairytale_d.py')");
#else
	PyRun_SimpleString("execfile('./scripts/fairytale.py')");
#endif
	system("pause");
	return 0;
}
