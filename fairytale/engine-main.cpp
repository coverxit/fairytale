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

// declared in "export-python.cpp"
extern "C" void initfairytale();

using namespace std;

int main()
{
	try
	{
		Py_Initialize();
		initfairytale();
		boost::python::import("scripts");
		Py_Finalize();
	}
	catch(Ogre::Exception& e)
    {
        cerr << "An Ogre::Exception has occurred: " << endl << e.getFullDescription();
		throw;
    }
	catch(std::exception& e)
	{
		cerr << "A std::exception has occurred: " << endl << e.what();
		throw;
	}
	catch(boost::python::error_already_set&)
	{
		cerr << "A python exception has occurred" << endl;
		throw;
	}
	return 0;
}
