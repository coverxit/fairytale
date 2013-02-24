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
#include "game-terrain.h"

using namespace fairytale;
using namespace Sqrat;

typedef void (*void_func_ptr_Application_const_ref_str)(Application*, const std::string&);
typedef void (*void_func_void)(void);

void BindSquirrel(SQVM* vm)
{
	RootTable(vm).Bind("Application", Class<Application>()
		.Func("initOgre",						&Application::initOgre)
		.Func("showConfigDialog",				&Application::showConfigDialog)
		.Func("initWindow",						&Application::initWindow)
		.Func("shutdown",						&Application::shutdown)
		.Func("addResourceLocation",			&Application::addResourceLocation)
		.Func("loadPlugin",						&Application::loadPlugin)
		.Func("loadPluginsFromDirectory",		&Application::loadPluginsFromDirectory)
		);

	RootTable(vm).Bind("vtt", Class<VolumeTerrainTest>()
		);
}
