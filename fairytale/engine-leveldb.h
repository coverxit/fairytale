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

#ifndef __FAIRYTALE_ENGINE_LEVELDB_H__
#define __FAIRYTALE_ENGINE_LEVELDB_H__

#include "engine-pch.h"
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/serialization/singleton.hpp>

namespace fairytale
{
	class ConfigFile
	{
	public:
		typedef boost::shared_ptr<leveldb::Iterator> IteratorPtr;

		ConfigFile();
		ConfigFile(const std::string& filename);

		void set(const std::string& key, const std::string& value);
		std::string get(const std::string& key);
		void remove(const std::string& key);
		void listAll();

	private:
		boost::shared_ptr<leveldb::DB> _db;
	};

	class ConfigManager
	{
	public:
		typedef boost::shared_ptr<ConfigFile> ConfigPtr;
		typedef boost::unordered_map<std::string, ConfigPtr> ConfigMap;

		static ConfigPtr get(const std::string& category);
		static void unload(const std::string& category);
		static void unloadAll();

	private:
		static ConfigMap _defaultmap;
	};
}

#endif
