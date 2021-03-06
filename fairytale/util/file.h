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

#ifndef __FAIRYTALE_UTIL_FILE_H__
#define __FAIRYTALE_UTIL_FILE_H__

#include <string>
#include <boost/function.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace fairytale { namespace util {

	enum class DirectoryProcessMode
	{
		ONLY_FILE,
		ONLY_DIRECTORY,
		ALL
	};

	void processDirectory(const std::string& dir, boost::function<void(const std::string&)> action, DirectoryProcessMode mode, bool includesubdirs);
	void processFilesInDirectory(const std::string& dir, boost::function<void(const std::string&)> action, bool includesubdirs, const std::string& filefilter = "");

} }

#endif
