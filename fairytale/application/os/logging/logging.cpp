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

#include "logging.h"

#include <memory>
#include <g2log/crashhandler.h>

#include "logic/interpreter/chaiscript.h"

namespace fairytale {

	LogManager::LogManager() : _mLogger("fairytale", "/logs/")
	{
		g2::initializeLogging(&_mLogger);
		g2::installSignalHandler();

		LOG(INFO) << "Logger loaded.";

		using namespace chaiscript;
		auto vm = ChaiScriptVM::getInstance().getVM();

		vm->add(chaiscript::user_type<g2::internal::LogMessage>(), "Logger");
		vm->add(chaiscript::fun(&g2::internal::LogMessage::messageStream), "messageStream");

		typedef g2::internal::LogMessage Logger;
		typedef std::shared_ptr<Logger> LoggerPtr;

		vm->add(chaiscript::fun([](const std::string& level) { return LoggerPtr(new Logger("userscript", 0 , "userscript", level)); }), "log");
	}
	
	LogManager::~LogManager()
	{
		LOG(INFO) << "Logger unloaded.";
		g2::shutDownLogging();
	}

}
