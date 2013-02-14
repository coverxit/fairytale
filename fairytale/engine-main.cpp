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

#include "engine-pch.h"

#include "engine-application.h"

// declared in "export-python.cpp"
extern "C" PyObject* PyInit_fairytale();
extern "C" PyObject* PyInit_OIS();

using namespace std;
using namespace fairytale;

int main()
{
	try
	{
		PyImport_AppendInittab("fairytale",	PyInit_fairytale);
		PyImport_AppendInittab("OIS",		PyInit_OIS);
		Py_Initialize();
		boost::python::import("scripts.beforeStartingMainLoop");
		Application().startLoop();
		Py_Finalize();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << e.what() << std::endl;
		throw;
	}
	catch(boost::python::error_already_set)
	{
		if(PyErr_Occurred())
			PyErr_Print();
		throw;
	}
	return 0;
}
