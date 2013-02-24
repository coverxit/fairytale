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

#include "engine-pch.h"
#include "engine-application.h"
#include "game-env.h"

namespace fairytale
{
	Environment::Sky::Sky() :
		_basiccontroller(new SkyX::BasicController()),
		_skyx(new SkyX::SkyX(Application::debugSceneMgr, _basiccontroller))
	{
		_skyx->create();

		// Distance geometry falling is a feature introduced in SkyX 0.2
		// When distance falling is enabled, the geometry linearly falls with the distance and the
		// amount of falling in world units is determinated by the distance between the cloud field "plane"
		// and the camera height multiplied by the falling factor.
		// For this demo, a falling factor of two is good enough for the point of view we're using. That means that if the camera
		// is at a distance of 100 world units from the cloud field, the fartest geometry will fall 2*100 = 200 world units.
		// This way the cloud field covers a big part of the sky even if the camera is in at a very low altitude.
		// The second parameter is the max amount of falling distance in world units. That's needed when for example, you've an 
		// ocean and you don't want to have the volumetric cloud field geometry falling into the water when the camera is underwater.
		// -1 means that there's not falling limit.
		_skyx->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));

		Application::ogreRoot->addFrameListener(_skyx.get());
		Application::renderWnd->addListener(_skyx.get());
	}

	void Environment::Sky::setWeather(const Weather& weather)
	{
		_skyx->setTimeMultiplier(weather.timeMultiplier);
		_basiccontroller->setTime(weather.time);
		_basiccontroller->setMoonPhase(weather.moonPhase);
		_skyx->getAtmosphereManager()->setOptions(weather.atmosphereOpt);

		// Layered clouds
		if(weather.layeredClouds)
		{
			// Create layer cloud
			if (_skyx->getCloudsManager()->getCloudLayers().empty())
			{
				_skyx->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
			}
		}
		else
		{
			// Remove layer cloud
			if (!_skyx->getCloudsManager()->getCloudLayers().empty())
			{
				_skyx->getCloudsManager()->removeAll();
			}
		}

		_skyx->getVCloudsManager()->setWindSpeed(weather.vcWindSpeed);
		_skyx->getVCloudsManager()->setAutoupdate(weather.vcAutoupdate);

		SkyX::VClouds::VClouds* vclouds = _skyx->getVCloudsManager()->getVClouds();

		vclouds->setWindDirection(weather.vcWindDir);
		vclouds->setAmbientColor(weather.vcAmbientColor);
		vclouds->setLightResponse(weather.vcLightResponse);
		vclouds->setAmbientFactors(weather.vcAmbientFactors);
		vclouds->setWheater(weather.vcWheater.x, weather.vcWheater.y, false);

		if (weather.volumetricClouds)
		{
			// Create VClouds
			if (!_skyx->getVCloudsManager()->isCreated())
			{
				// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
				_skyx->getVCloudsManager()->create(_skyx->getMeshManager()->getSkydomeRadius(Application::deaultCam));
			}
		}
		else
		{
			// Remove VClouds
			if (_skyx->getVCloudsManager()->isCreated())
			{
				_skyx->getVCloudsManager()->remove();
			}
		}

		vclouds->getLightningManager()->setEnabled(weather.vcLightnings);
		vclouds->getLightningManager()->setAverageLightningApparitionTime(weather.vcLightningsAT);
		vclouds->getLightningManager()->setLightningColor(weather.vcLightningsColor);
		vclouds->getLightningManager()->setLightningTimeMultiplier(weather.vcLightningsTM);

		_skyx->update(0);
	}
}
