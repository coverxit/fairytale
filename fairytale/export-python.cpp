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
#include "engine-leveldb.h"

using namespace fairytale;
using namespace boost::python;

struct ApplicationExport
{
	static void initEngine(const std::string& wndtitle)
	{
		Application::get_mutable_instance().initOgre(wndtitle, 0, 0);
	}

	static void startLoop()
	{
		Application::get_mutable_instance().startLoop();
	}
};

BOOST_PYTHON_MODULE(fairytale)
{
	class_<ApplicationExport>("Application")
		.def("initEngine",			&ApplicationExport::initEngine)		.staticmethod("initEngine")
		.def("startLoop",			&ApplicationExport::startLoop)		.staticmethod("startLoop")
		;

	class_<ConfigFile>("ConfigFile", init<const std::string&>())
		.def("set",					&ConfigFile::set)
		.def("get",					&ConfigFile::get)
		.def("remove",				&ConfigFile::remove)
		.def("listAll",				&ConfigFile::listAll)
		;

	register_ptr_to_python<ConfigManager::ConfigPtr>();
	class_<ConfigManager>("ConfigManager")
		.def("get",					&ConfigManager::get)				.staticmethod("get")
		.def("unload",				&ConfigManager::unload)				.staticmethod("unload")
		.def("unloadAll",			&ConfigManager::unloadAll)			.staticmethod("unloadAll")
		;
}
