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
#include "engine-leveldb.h"

namespace fairytale
{
	ConfigFile::ConfigFile()
	{
	}

	ConfigFile::ConfigFile(const std::string& filename)
	{
		leveldb::DB* tempdbpointer;
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, filename, &tempdbpointer);
		assert(status.ok());
		_db.reset(tempdbpointer);
	}

	void ConfigFile::set(const std::string& key, const std::string& value)
	{
		leveldb::Status status = _db->Put(leveldb::WriteOptions(), key, value);
		assert(status.ok());
	}

	std::string ConfigFile::get(const std::string& key)
	{
		std::string dest;
		leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &dest);
		assert(status.ok());
		return dest;
	}

	void ConfigFile::remove(const std::string& key)
	{
		leveldb::Status status = _db->Delete(leveldb::WriteOptions(), key);
		assert(status.ok());
	}

	void ConfigFile::listAll()
	{
		boost::scoped_ptr<leveldb::Iterator> it(_db->NewIterator(leveldb::ReadOptions()));
		for(it->SeekToFirst(); it->Valid(); it->Next())
		{
			std::cout << it->key().ToString() << ": "  << it->value().ToString() << std::endl;
		}
	}

	ConfigManager::ConfigMap ConfigManager::_defaultmap = ConfigManager::ConfigMap();

	ConfigManager::ConfigPtr ConfigManager::get(const std::string& category)
	{
		ConfigMap::iterator iter(_defaultmap.find(category));
		if(iter == _defaultmap.end())
			iter = _defaultmap.insert(std::make_pair(category, ConfigPtr(new ConfigFile("./data/" + category)))).first;
		return iter->second;
	}

	void ConfigManager::unload(const std::string& category)
	{
		ConfigMap::iterator iter(_defaultmap.find(category));
		assert(iter != _defaultmap.end());
		_defaultmap.erase(iter);
	}

	void ConfigManager::unloadAll()
	{
		_defaultmap.clear();
	}
}
