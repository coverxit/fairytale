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

// declared in "export-squirrel.cpp"
extern void BindSquirrel(HSQUIRRELVM vm);

using namespace std;
using namespace fairytale;
using namespace Sqrat;

void printFunc(HSQUIRRELVM v,const SQChar * s,...)
{
	SQChar temp[2048];
	va_list vl;
	va_start(vl, s);
	scvsprintf(temp, s, vl);
	va_end(vl);

	Ogre::LogManager::getSingleton().stream() << temp;
}

int main()
{
	HSQUIRRELVM vm = sq_open(1024);
	sq_setprintfunc(vm, printFunc, printFunc);
	sq_seterrorhandler(vm);
	DefaultVM::Set(vm);

	try
	{
		BindSquirrel(vm);
		Script tmp;
		tmp.CompileFile("scripts/beforeStartingMainLoop.nut");
		tmp.Run();
		Application().startLoop();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(Exception& e)
	{
		std::cout << e.Message() << std::endl;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{
	}

	sq_close(vm);
	// Windows only...
	system("pause");

	return 0;
}
