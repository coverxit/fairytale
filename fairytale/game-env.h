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

#ifndef __FAIRYTALE_GAME_ENV_H__
#define __FAIRYTALE_GAME_ENV_H__

#include "engine-pch.h"
#include "util-singleton.h"
#include "game-env-data.h"

namespace fairytale
{
	namespace Environment
	{
		class Sky // : public Singleton<Sky>
		{
		protected:
			SkyX::BasicController* _basiccontroller;
			boost::shared_ptr<SkyX::SkyX> _skyx;

		public:
			Sky();

			//void registerWeatherType(const std::string& name, const Weather& weather);
			//void unregisterWeatherType(const std::string& name);
			//Weather getWeatherData(const std::string& name);
			//void setWeather(const std::string& name);

			void setWeather(const Weather& weather);
		};
	}
}

#endif
