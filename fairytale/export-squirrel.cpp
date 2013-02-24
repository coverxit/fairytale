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

#include "engine-pch.h"

#include "engine-application.h"
#include "game-physics.h"
#include "game-scene.h"
#include "game-terrain.h"

using namespace fairytale;
using namespace Sqrat;

typedef void (*void_func_const_ref_str)(const std::string&);
typedef void (*void_func_void)(void);

void BindSquirrel(SQVM* vm)
{
	RootTable(vm).Bind("Application", Table()
		.Func("init",							(void_func_void)(&Application::getInstance))
		.Func("addResourceLocation",			(void_func_const_ref_str)([](const std::string& str1) -> void { Application::getInstance().addResourceLocation(str1); }))
		.Func("loadPlugin",						(void_func_const_ref_str)([](const std::string& str1) -> void { Application::getInstance().loadPlugin(str1); }))
		.Func("loadPluginsFromDirectory",		(void_func_const_ref_str)([](const std::string& str1) -> void { Application::getInstance().loadPluginsFromDirectory(str1); }))
		.Func("shutdown",						(void_func_void)([]() -> void { Application::getInstance().shutdown(); }))
		);

	RootTable(vm).Bind("Physics", Table()
		.Func("init",							(void_func_void)(&GamePhysics::getInstance))
		);

	RootTable(vm).Bind("Scene", Table()
		.Func("init",							(void_func_void)(&GameScene::getInstance))
		);

	RootTable(vm).Bind("vtt", Class<VolumeTerrainTest>()
		);
}
