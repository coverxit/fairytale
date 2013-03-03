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

#include "pch.h"

#include <sqrat.h>

#include "engine/graphics.h"
#include "engine/input.h"
#include "engine/physics.h"

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

boost::scoped_ptr<boost::thread>	gameConsole;
std::deque<std::string>				commands;
boost::mutex						commandsDequeMutex;

void waitForUserInput()
{
	std::string cmd;
	std::cout << "Console is now available!\n>>> ";
	while(!engine::graphics::renderingStopped())
	{
		std::getline(std::cin, cmd);
		boost::mutex::scoped_lock(commandsDequeMutex);
		commands.push_back(cmd);
		std::cin.clear();
		fflush(stdin);
	}
}

void doMainLoop()
{
	Ogre::LogManager::getSingleton().stream() << "Main loop entered";

	gameConsole.reset(new boost::thread(waitForUserInput));

	while(!engine::graphics::renderingStopped() && !engine::graphics::getDefaultRenderWindow()->isClosed())
	{
		// process commands
		{
			boost::mutex::scoped_lock(commandsDequeMutex);
			while(!commands.empty())
			{
				try
				{
					Sqrat::Script script;
					script.CompileString(commands.front());
					script.Run();
				}
				catch(Sqrat::Exception& e)
				{
					std::cout << e.Message() << std::endl;
				}
				std::cout << ">>> ";
				commands.pop_front();
			}
		}

		{
			Ogre::WindowEventUtilities::messagePump();

			engine::input::getKeyboard()->capture();
			engine::input::getMouse()->capture();

			engine::graphics::getOGRE()->renderOneFrame();
		}
	}

	Ogre::LogManager::getSingleton().stream() << "Main loop quit";
}

int main()
{
	HSQUIRRELVM vm = sq_open(1024);
	sq_setprintfunc(vm, printFunc, printFunc);
	sq_seterrorhandler(vm);
	DefaultVM::Set(vm);

	engine::graphics::initOgre();

	size_t hWnd = 0;
	engine::graphics::getDefaultRenderWindow()->getCustomAttribute("WINDOW", &hWnd);

	engine::input::initOIS(hWnd);
	engine::input::setMouseBufferedArea(engine::graphics::getDefaultRenderWindow()->getWidth(), engine::graphics::getDefaultRenderWindow()->getHeight());

	engine::input::registerKeyBinding("__basic_screenshot", OIS::KC_SYSRQ, engine::input::KeyState::DOWN, [](const OIS::KeyEvent&) {
		engine::graphics::takeScreenshot();
	});

	engine::input::registerKeyBinding("__basic_exit", OIS::KC_ESCAPE, engine::input::KeyState::DOWN, [](const OIS::KeyEvent&) {
		engine::graphics::stopRendering();
	});

	engine::physics::initPhysicsWorld();
	engine::graphics::getOGRE()->addFrameListener(engine::physics::getPhysicsFrameListener());

	try
	{
		BindSquirrel(vm);
		Script tmp;
		tmp.CompileFile("scripts/beforeStartingMainLoop.nut");
		tmp.Run();
		doMainLoop();
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
