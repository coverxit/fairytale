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

#include "file.h"

namespace fairytale { namespace util {

	void processDirectory(const std::string& dir, boost::function<void(const std::string&)> action, DirectoryProcessMode mode, bool includesubdirs)
	{
		using namespace boost::filesystem;
		path root(dir, native);

		if(exists(root))
		{
			directory_iterator end;
			boost::smatch what;
			for(directory_iterator iter(root); iter != end; ++iter)
			{
				if(is_directory(*iter) && includesubdirs)
				{
					processDirectory(iter->path().string(), action, mode, includesubdirs);
				}
				if((mode == ALL) || (mode == ONLY_FILE && is_regular_file(*iter)) || (mode == ONLY_DIRECTORY && is_directory(*iter)))
				{
					action(iter->path().string());
				}
			}
		}
	}

	void processFilesInDirectory(const std::string& dir, boost::function<void(const std::string&)> action, bool includesubdirs, const std::string& filefilter)
	{
		boost::smatch what;
		processDirectory(dir, [&what, &filefilter, &action](const std::string& path) {
			if(filefilter.empty() || boost::regex_match(path, what, boost::regex(filefilter)))
				action(path);
		}, ONLY_FILE, includesubdirs);
	}

} }
