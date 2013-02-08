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

#ifndef __FAIRYTALE_UTIL_SERIALIZATION_H__
#define __FAIRYTALE_UTIL_SERIALIZATION_H__

#include "game-env.h"

namespace boost
{
	namespace serialization
	{
		template<class Archive>
		void serialize(Archive& ar, fairytale::Environment::Weather& i, const unsigned int version)
		{
			ar & i.atmosphereOpt;
			ar & i.layeredClouds;
			ar & i.moonPhase;
			ar & i.time;
			ar & i.timeMultiplier;
			ar & i.vcAmbientColor;
			ar & i.vcAmbientFactors;
			ar & i.vcAutoupdate;
			ar & i.vcLightnings;
			ar & i.vcLightningsAT;
			ar & i.vcLightningsColor;
			ar & i.vcLightningsTM;
			ar & i.vcLightResponse;
			ar & i.vcWheater;
			ar & i.vcWindDir;
			ar & i.vcWindSpeed;
			ar & i.volumetricClouds;
		}

		template<class Archive>
		void serialize(Archive& ar, SkyX::AtmosphereManager::Options& i, const unsigned int version)
		{
			ar & i.Exposure;
			ar & i.G;
			ar & i.HeightPosition;
			ar & i.InnerRadius;
			ar & i.MieMultiplier;
			ar & i.NumberOfSamples;
			ar & i.OuterRadius;
			ar & i.RayleighMultiplier;
			ar & i.SunIntensity;
			ar & i.WaveLength;
		}

		template<class Archive>
		void serialize(Archive& ar, Ogre::Radian& i, const unsigned int version)
		{
			ar & i.mRad;
		}

		template<class Archive>
		void serialize(Archive& ar, Ogre::Vector2& i, const unsigned int version)
		{
			ar & i.x;
			ar & i.y;
		}

		template<class Archive>
		void serialize(Archive& ar, Ogre::Vector3& i, const unsigned int version)
		{
			ar & i.x;
			ar & i.y;
			ar & i.z;
		}

		template<class Archive>
		void serialize(Archive& ar, Ogre::Vector4& i, const unsigned int version)
		{
			ar & i.x;
			ar & i.y;
			ar & i.z;
			ar & i.w;
		}
	}
}

#endif
