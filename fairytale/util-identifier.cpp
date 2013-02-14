/*
 * Copyright (C) 2012 Incognito
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

// This file is modified to be ready-to-use. Thanks to Incognito and his awesome project
// samp-streamer-plugin. See original file at https://code.google.com/p/samp-streamer-plugin/
// TennenColl 2013

#include "util-identifier.h"

namespace fairytale
{
	Identifier::Identifier()
	{
		highestID = 0;
	}
	
	int Identifier::get()
	{
		int id = 0;
		if (!removedIDs.empty())
		{
			id = removedIDs.top();
			removedIDs.pop();
		}
		else
		{
			id = ++highestID;
		}
		return id;
	}
	
	void Identifier::remove(int id, std::size_t remaining)
	{
		if (remaining > 1)
		{
			removedIDs.push(id);
		}
		else
		{
			reset();
		}
	}
	
	void Identifier::reset()
	{
		highestID = 0;
		removedIDs = std::priority_queue<int, std::vector<int>, std::greater<int> >();
	}
}
