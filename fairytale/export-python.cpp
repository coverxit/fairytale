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
#include "engine-input.h"

using namespace fairytale;
using namespace boost::python;

typedef void (*func_ptr_Application_const_ref_str)(Application*, const std::string&);

BOOST_PYTHON_MODULE(fairytale)
{
	class_<Application>("Application")
		.def("initOgre",						&Application::initOgre)
		.def("showConfigDialog",				&Application::showConfigDialog)
		.def("initWindow",						&Application::initWindow)
		.def("shutdown",						&Application::shutdown)
		.def("addResourceLocation",				&Application::addResourceLocation)
		.def("addResourceLocationsFromFile",	&Application::addResourceLocationsFromFile)
		.def("loadPlugin",						&Application::loadPlugin)
		.def("loadPluginsFromDirectory",		&Application::loadPluginsFromDirectory)
		;
}
