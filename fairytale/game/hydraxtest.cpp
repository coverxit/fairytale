/*
--------------------------------------------------------------------------------
This source file is part of Hydrax-SkyX demo.
Visit ---

Copyright (C) 2009 Xavier Verguín González 'Xavyiy'
				   <xavierverguin@hotmail.com> <xavyiy@gmail.com>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/
--------------------------------------------------------------------------------
*/

// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>
#include <Ogre/Overlay/OgreTextAreaOverlayElement.h>

// ----------------------------------------------------------------------------
// Include the OGRE example framework
// This includes the classes defined to make getting an OGRE application running
// a lot easier. It automatically sets up all the main objects and allows you to
// just override the bits you want to instead of writing it all from scratch.
// ----------------------------------------------------------------------------
#include <ExampleApplication.h>

// ----------------------------------------------------------------------------
// Include the Hydrax plugin headers
// Main base headers (Hydrax.h) and especific headers (Noise/Water modules)
// ----------------------------------------------------------------------------
#include <Hydrax/Hydrax.h>
#include <Hydrax/Noise/Perlin/Perlin.h>
#include <Hydrax/Modules/ProjectedGrid/ProjectedGrid.h>

// ----------------------------------------------------------------------------
// Include the SkyX plugin headers
// ----------------------------------------------------------------------------
#include<SkyX.h>

// Hydrax object
Hydrax::Hydrax *mHydrax = 0;
// SkyX object
SkyX::SkyX *mSkyX = 0;

// Just to show information
Ogre::TextAreaOverlayElement* mTextArea = 0;
bool mShowInformation = false;
bool mBloomCompositor = true;

// Shadow config struct
struct ShadowConfig
{
	bool Enable;
	int  Size;

	ShadowConfig(const bool& Enable_, const int& Size_)
		: Enable(Enable_)
		, Size(Size_)
	{
	}
};
// Shadow mode
enum ShadowMode
{
	SM_NONE = 0,
	SM_LOW  = 1,
	SM_HIGH = 2
};
// Shadow config list
ShadowConfig mShadowConfigList[] = {ShadowConfig(false, 0), ShadowConfig(true, 512), ShadowConfig(true, 2048)};
// Current shadow mode
int mShadowMode = static_cast<int>(SM_LOW);
// To disable shadows in night
bool mForceDisableShadows = false;

// Time multiplier
Ogre::Real mTimeMultiplier = 0.1f;

/** Builds a string that contains current options
 */
Ogre::String buildConfigString(const SkyX::AtmosphereManager::Options& Options)
{
	int hour = static_cast<int>(Options.Time.x);
	int min  = static_cast<int>((Options.Time.x - hour)*60);

	Ogre::String timeStr = Ogre::StringConverter::toString(hour) + ":" + Ogre::StringConverter::toString(min);
	Ogre::String str = "Hydrax-SkyX demo (Press F1 to show/hide information)" + static_cast<Ogre::String>(mShowInformation ? " - Simuation paused - \n" : "\n-------------------------------------------------------------\nTime: " + timeStr + "\n");

	if (mShowInformation)
	{
		str += "-------------------------------------------------------------\n";
		str += "Time: " + timeStr + " [1, Shift+1] (+/-).\n";
		str += "Rayleigh multiplier: " + Ogre::StringConverter::toString(Options.RayleighMultiplier) + " [2, Shift+2] (+/-).\n";
		str += "Mie multiplier: " + Ogre::StringConverter::toString(Options.MieMultiplier) + " [3, Shift+3] (+/-).\n";
		str += "Exposure: " + Ogre::StringConverter::toString(Options.Exposure) + " [4, Shift+4] (+/-).\n";
		str += "Inner radius: " + Ogre::StringConverter::toString(Options.InnerRadius) + " [5, Shift+5] (+/-).\n";
		str += "Outer radius: " + Ogre::StringConverter::toString(Options.OuterRadius) + " [6, Shift+6] (+/-).\n";
		str += "Number of samples: " + Ogre::StringConverter::toString(Options.NumberOfSamples) + " [7, Shift+7] (+/-).\n";
		str += "Height position: " + Ogre::StringConverter::toString(Options.HeightPosition) + " [8, Shift+8] (+/-).\n";
		str += "Time multiplier: " + Ogre::StringConverter::toString(mTimeMultiplier) + " [9, Shift+9] (+/-).\n";
	}

	str += "-------------------------------------------------------------\n";
	str += "Bloom effect "+static_cast<Ogre::String>(mBloomCompositor ? "- Enabled " : "- Disabed ") +"[F2]\n";

	switch (static_cast<ShadowMode>(mShadowMode))
	{
		case SM_NONE:
		{
			str += "Shadows - Disabled [F3]";
		}
		break;

		case SM_LOW:
		{
			str += "Shadows - Low quality [F3]";
		}
		break;

		case SM_HIGH:
		{
			str += "Shadows - High quality [F3]";
		}
		break;
	}

	return str;
}

/** Set shadow mode
 */
void setShadowMode(Ogre::SceneManager *sm, const ShadowMode& smode)
{
	Ogre::MaterialPtr IslandMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"));

	if (mShadowConfigList[smode].Enable && !mForceDisableShadows)
	{
		sm->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
		sm->setShadowTextureConfig(0, mShadowConfigList[smode].Size, mShadowConfigList[smode].Size, Ogre::PF_X8R8G8B8);
		IslandMat->getTechnique(0)->setSchemeName("Default");
		IslandMat->getTechnique(1)->setSchemeName("NoDefault");
	}
	else
	{
		sm->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		IslandMat->getTechnique(0)->setSchemeName("NoDefault");
		IslandMat->getTechnique(1)->setSchemeName("Default");
	}
}

// ----------------------------------------------------------------------------
// Define the application object
// This is derived from ExampleApplication which is the class OGRE provides to
// make it easier to set up OGRE without rewriting the same code all the time.
// You can override extra methods of ExampleApplication if you want to further
// specialise the setup routine, otherwise the only mandatory override is the
// 'createScene' method which is where you set up your own personal scene.
// ----------------------------------------------------------------------------

class ExampleHydraxDemoListener : public ExampleFrameListener
{
public:
    SceneManager *mSceneMgr;
    Real mKeyBuffer;
	Real mLastPositionLength;

	// Color gradients
	SkyX::ColorGradient mWaterGradient, 
		                mSunGradient, 
						mAmbientGradient;

    ExampleHydraxDemoListener(RenderWindow* win, Camera* cam, SceneManager *sm)
            : ExampleFrameListener(win,cam)
            , mSceneMgr(sm)
            , mKeyBuffer(-1)
			, mLastPositionLength((Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length())
    {
		// Color gradients
		// Water
		mWaterGradient = SkyX::ColorGradient();
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.779105)*0.4, 1));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.729105)*0.3, 0.8));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.25, 0.6));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.2, 0.5));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.1, 0.45));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.025, 0));
		// Sun
		mSunGradient = SkyX::ColorGradient();
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.5, 1.0f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.4, 0.75f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.25, 0.45f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.01, 0.0f));
		// Ambient
		mAmbientGradient = SkyX::ColorGradient();
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 1.0f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 0.6f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.6, 0.5f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.3, 0.45f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.1, 0.35f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.05, 0.0f));
    }

    bool frameStarted(const FrameEvent &e)
    {
		mKeyboard->capture();

		// Check camera height
		Ogre::RaySceneQuery *raySceneQuery = 
			mSceneMgr->
			     createRayQuery(Ogre::Ray(mCamera->getPosition() + Ogre::Vector3(0,1000000,0), 
				                Vector3::NEGATIVE_UNIT_Y));
		Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
        Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
        if (i != qryResult.end() && i->worldFragment)
        {
			if (mCamera->getPosition().y < i->worldFragment->singleIntersection.y + 30)
			{
                mCamera->
				     setPosition(mCamera->getPosition().x, 
                                 i->worldFragment->singleIntersection.y + 30, 
                                 mCamera->getPosition().z);
			}
        }

		delete raySceneQuery;

		// Change SkyX atmosphere options if needed
		SkyX::AtmosphereManager::Options SkyXOptions = mSkyX->getAtmosphereManager()->getOptions();

		// Time
		if (!mShowInformation)
		{
			mSkyX->setTimeMultiplier(mTimeMultiplier);
		}
		else
		{
			mSkyX->setTimeMultiplier(0.0f);
		}

		if (mKeyboard->isKeyDown(OIS::KC_1) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			mSkyX->setTimeMultiplier(1.0f);
		if (mKeyboard->isKeyDown(OIS::KC_1) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			mSkyX->setTimeMultiplier(-1.0f);

		// Rayleigh multiplier
		if (mKeyboard->isKeyDown(OIS::KC_2) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.RayleighMultiplier += e.timeSinceLastFrame*0.025f;
		if (mKeyboard->isKeyDown(OIS::KC_2) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.RayleighMultiplier -= e.timeSinceLastFrame*0.025f;

		// Mie multiplier
		if (mKeyboard->isKeyDown(OIS::KC_3) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.MieMultiplier += e.timeSinceLastFrame*0.025f;
		if (mKeyboard->isKeyDown(OIS::KC_3) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.MieMultiplier -= e.timeSinceLastFrame*0.025f;

		// Exposure
		if (mKeyboard->isKeyDown(OIS::KC_4) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.Exposure += e.timeSinceLastFrame*0.5f;
		if (mKeyboard->isKeyDown(OIS::KC_4) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.Exposure -= e.timeSinceLastFrame*0.5f;

		// Inner radius
		if (mKeyboard->isKeyDown(OIS::KC_5) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.InnerRadius += e.timeSinceLastFrame*0.25f;
		if (mKeyboard->isKeyDown(OIS::KC_5) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.InnerRadius -= e.timeSinceLastFrame*0.25f;

		// Outer radius
		if (mKeyboard->isKeyDown(OIS::KC_6) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.OuterRadius += e.timeSinceLastFrame*0.25f;
		if (mKeyboard->isKeyDown(OIS::KC_6) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.OuterRadius -= e.timeSinceLastFrame*0.25f;

		// Number of samples
		if (mKeyboard->isKeyDown(OIS::KC_7) && mKeyBuffer < 0 &&!(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
		{
			SkyXOptions.NumberOfSamples ++;
			mKeyBuffer = 0.25f;
		}
		if (mKeyboard->isKeyDown(OIS::KC_7) && mKeyBuffer < 0 && (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
		{
			SkyXOptions.NumberOfSamples --;
			mKeyBuffer = 0.25f;
		}

		// Height position
		if (mKeyboard->isKeyDown(OIS::KC_8) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.HeightPosition += e.timeSinceLastFrame*0.05f;
		if (mKeyboard->isKeyDown(OIS::KC_8) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			SkyXOptions.HeightPosition -= e.timeSinceLastFrame*0.05f;

		// Time multiplier
		if (mKeyboard->isKeyDown(OIS::KC_9) && !(mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			mTimeMultiplier += e.timeSinceLastFrame*0.15f;
		if (mKeyboard->isKeyDown(OIS::KC_9) &&  (mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
			mTimeMultiplier -= e.timeSinceLastFrame*0.15f;

		mSkyX->getAtmosphereManager()->setOptions(SkyXOptions);

		// Show/Hide information
		if (mKeyboard->isKeyDown(OIS::KC_F1) && mKeyBuffer < 0)
        {
			mShowInformation = !mShowInformation;

			mKeyBuffer = 0.25f;
		}

		// Post-proccesing effects
		if (mKeyboard->isKeyDown(OIS::KC_F2) && mKeyBuffer < 0)
        {
			mBloomCompositor = !mBloomCompositor;

			Ogre::CompositorManager::getSingleton().
					setCompositorEnabled(mWindow->getViewport(0), "Bloom", mBloomCompositor);

			mKeyBuffer = 0.25f;
		}

		// Shadows
		if (mKeyboard->isKeyDown(OIS::KC_F3) && mKeyBuffer < 0)
        {
			mShadowMode++;

			if (mShadowMode > 2)
			{
				mShadowMode = 0;
			}

			setShadowMode(mSceneMgr, static_cast<ShadowMode>(mShadowMode));

			mKeyBuffer = 0.25f;
		}

		mTextArea->setCaption(buildConfigString(SkyXOptions));

		// Update environment lighting
		updateEnvironmentLighting();

		// Update shadow far distance
		updateShadowFarDistance();

		// Update Hydrax
        mHydrax->update(e.timeSinceLastFrame);
		// Update SkyX
		mSkyX->update(e.timeSinceLastFrame);

		mKeyBuffer -= e.timeSinceLastFrame;

        return true;
    }

	/** Update environment lighting
	 */
	void updateEnvironmentLighting()
	{
		Ogre::Vector3 lightDir = mSkyX->getAtmosphereManager()->getSunDirection();

		bool preForceDisableShadows = mForceDisableShadows;
		mForceDisableShadows = (lightDir.y > 0.15f) ? true : false;

		if (preForceDisableShadows != mForceDisableShadows)
		{
			setShadowMode(mSceneMgr, static_cast<ShadowMode>(mShadowMode));
		}

		// Calculate current color gradients point
		float point = (-lightDir.y + 1.0f) / 2.0f;
		mHydrax->setWaterColor(mWaterGradient.getColor(point));

		Ogre::Vector3 sunPos = mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.1;
		mHydrax->setSunPosition(sunPos);

		Ogre::Light *Light0 = mSceneMgr->getLight("Light0"),
					*Light1 = mSceneMgr->getLight("Light1");

		Light0->setPosition(mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.02);
		Light1->setDirection(lightDir);

		Ogre::Vector3 sunCol = mSunGradient.getColor(point);
		Light0->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
		Ogre::Vector3 ambientCol = mAmbientGradient.getColor(point);
		Light0->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);
		mHydrax->setSunColor(sunCol);
	}

	/** Update shadow far distance
	 */
	void updateShadowFarDistance()
	{
		Ogre::Light* Light1 = mSceneMgr->getLight("Light1");
		float currentLength = (Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length();

		if (currentLength < 1000)
		{
			mLastPositionLength = currentLength;
			return;
		}
		
		if (currentLength - mLastPositionLength > 100)
		{
			mLastPositionLength += 100;

			Light1->setShadowFarDistance(Light1->getShadowFarDistance() + 100);
		}
		else if (currentLength - mLastPositionLength < -100)
		{
			mLastPositionLength -= 100;

			Light1->setShadowFarDistance(Light1->getShadowFarDistance() - 100);
		}
	}
};

// ----------------------------------------------------------------------------
// Hydrax Rtt Listener class (Since Hydrax 0.5.1 version)
// ----------------------------------------------------------------------------
class HydraxRttListener : public Hydrax::RttManager::RttListener
{
public:
	void preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
	{
		// If needed in any case...
		bool underwater = mHydrax->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				// No stars in the reflection map
				mSkyX->setStarfieldEnabled(false);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				// Hide SkyX components in depth maps
				mSkyX->getMeshManager()->getEntity()->setVisible(false);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(false);
			}
			break;
		}
	}

	void postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
	{
		bool underwater = mHydrax->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				mSkyX->setStarfieldEnabled(true);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				mSkyX->getMeshManager()->getEntity()->setVisible(true);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(true);
			}
			break;
		}
	}
};

class BloomListener: public Ogre::CompositorInstance::Listener
{
public:
    void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
	}
    void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if (pass_id == 3)
		{
			float bloomStrength = 0.75 + Ogre::Math::Clamp<Ogre::Real>(-mSkyX->getAtmosphereManager()->getSunDirection().y, 0, 1)*0.75;
			mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uBloomStrength", bloomStrength);
		}
	}
};

/** Just to locate palmiers with a pseudo-random algoritm
 */
float seed_ = 801;
float rnd_(const float& min, const float& max)
{
	seed_ += Ogre::Math::PI*2.8574f + seed_*(0.3424f - 0.12434f + 0.452345f);
	if (seed_ > 10000000000) seed_ -= 10000000000;
	return ((max-min)*Ogre::Math::Abs(Ogre::Math::Sin(Ogre::Radian(seed_))) + min);
}

void createPalms(Ogre::SceneManager *mSceneMgr)
{
	const int NumberOfPalms = 12;

	Ogre::SceneNode* mPalmsSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		
	for (int k = 0; k < NumberOfPalms; k++)
	{
		Ogre::Vector3 RandomPos = Ogre::Vector3(rnd_(500,2500),
			0,
			rnd_(500,2500));

		Ogre::RaySceneQuery * raySceneQuery = mSceneMgr->
			createRayQuery(Ogre::Ray(RandomPos + Ogre::Vector3(0,1000000,0), 
			Ogre::Vector3::NEGATIVE_UNIT_Y));

		Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator i = qryResult.begin();

		if (i != qryResult.end() && i->worldFragment)
		{
			if (i->worldFragment->singleIntersection.y>105 || i->worldFragment->singleIntersection.y<20)
			{
				k--;
				continue;
			}

			RandomPos.y = i->worldFragment->singleIntersection.y;
		}
		else
		{
			k--;
			continue;
		}

		Ogre::Entity *mPalmEnt = mSceneMgr->createEntity("Palm"+Ogre::StringConverter::toString(k), "Palm.mesh");
		Ogre::SceneNode *mPalmSN = mPalmsSceneNode->createChildSceneNode();

		mPalmSN->rotate(Ogre::Vector3(-1,0,rnd_(-0.3,0.3)), Ogre::Degree(90));
		mPalmSN->attachObject(mPalmEnt);
		Ogre::Real Scale = rnd_(50,75);
		mPalmSN->scale(Scale,Scale,Scale);
		mPalmSN->setPosition(RandomPos);
	}
}

class SampleApp : public ExampleApplication
{
public:
    // Basic constructor
    SampleApp()
    {}

protected:
	void chooseSceneManager()
    {
        // Create the SceneManager, in this case a generic one
        mSceneMgr = mRoot->createSceneManager("TerrainSceneManager");
    }

    // Just override the mandatory create scene method
    void createScene(void)
    {
		// Set default ambient light
		mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

		// Set some camera params
        mCamera->setFarClipDistance(99999*6);
		mCamera->setPosition(311.902,128.419,1539.02);
		mCamera->setDirection(0.155, 0.1808, -0.97);

	    // Light
		Ogre::Light *mLight0 = mSceneMgr->createLight("Light0");
		mLight0->setDiffuseColour(1, 1, 1);
		mLight0->setCastShadows(false);

		// Shadow caster
		Ogre::Light *mLight1 = mSceneMgr->createLight("Light1");
		mLight1->setType(Ogre::Light::LT_DIRECTIONAL);

		// Hydrax initialization code ---------------------------------------------
		// ------------------------------------------------------------------------

        // Create Hydrax object
		mHydrax = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));

		// Create our projected grid module  
		Hydrax::Module::ProjectedGrid *mModule 
			= new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
			                                    mHydrax,
												// Noise module
			                                    new Hydrax::Noise::Perlin(/*Generic one*/),
												// Base plane
			                                    Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
												// Normal mode
												Hydrax::MaterialManager::NM_VERTEX,
												// Projected grid options
										        Hydrax::Module::ProjectedGrid::Options(/*Generic one*/));

		// Set our module
		mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

		// Load all parameters from config file
		// Remarks: The config file must be in Hydrax resource group.
		// All parameters can be set/updated directly by code(Like previous versions),
		// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
		mHydrax->loadCfg("HydraxDemo.hdx");

        // Create water
        mHydrax->create();

		// Add the Hydrax Rtt listener
		mHydrax->getRttManager()->addRttListener(new HydraxRttListener());

		// Hydrax initialization code end -----------------------------------------
		// ------------------------------------------------------------------------

		// SkyX initialization code ---------------------------------------------
		// ------------------------------------------------------------------------

		// Create SkyX object
		mSkyX = new SkyX::SkyX(mSceneMgr, mCamera);

		// No smooth fading
		mSkyX->getMeshManager()->setSkydomeFadingParameters(false);

		// A little change to default atmosphere settings :)
		SkyX::AtmosphereManager::Options atOpt = mSkyX->getAtmosphereManager()->getOptions();
		atOpt.RayleighMultiplier = 0.003075f;
		atOpt.MieMultiplier = 0.00125f;
		atOpt.InnerRadius = 9.92f;
		atOpt.OuterRadius = 10.3311f;
		mSkyX->getAtmosphereManager()->setOptions(atOpt);

		// Create the sky
		mSkyX->create();

		// Add a basic cloud layer
		mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));

		// SkyX initialization code end -----------------------------------------
		// ------------------------------------------------------------------------

		// Bloom compositor
		Ogre::CompositorManager::getSingleton().
			addCompositor(mWindow->getViewport(0), "Bloom")->addListener(new BloomListener());
		Ogre::CompositorManager::getSingleton().
			setCompositorEnabled(mWindow->getViewport(0), "Bloom", mBloomCompositor);

		// Shadows
		mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
		mSceneMgr->setShadowTextureCasterMaterial("ShadowCaster");
		mSceneMgr->getLight("Light1")->setShadowFarDistance(1750);
		setShadowMode(mSceneMgr, static_cast<ShadowMode>(mShadowMode));

		// Load island
		mSceneMgr->setWorldGeometry("Island.cfg");

		// Add the Hydrax depth technique to island material
		mHydrax->getMaterialManager()->addDepthTechnique(
			static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))
			->createTechnique());

		// Create palmiers
		createPalms(mSceneMgr);

		// Create text area to show skyboxes information
		createTextArea();
		
		// Add frame listener
		mRoot->addFrameListener(new ExampleHydraxDemoListener(mWindow, mCamera, mSceneMgr));
    }

	// Create text area to show skyboxes information
	void createTextArea()
	{
		// Create a panel
		Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(
			OverlayManager::getSingleton().createOverlayElement("Panel", "HydraxSkyXDemoInformationPanel"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(400, 400);

		// Create a text area
		mTextArea = static_cast<Ogre::TextAreaOverlayElement*>(
			OverlayManager::getSingleton().createOverlayElement("TextArea", "HydraxSkyXDemoInformationTextArea"));
		mTextArea->setMetricsMode(Ogre::GMM_PIXELS);
		mTextArea->setPosition(0, 0);
		mTextArea->setDimensions(100, 100);
		mTextArea->setCharHeight(16);
		mTextArea->setFontName("BlueHighway");
		mTextArea->setColourBottom(ColourValue(0.3, 0.5, 0.3));
		mTextArea->setColourTop(ColourValue(0.5, 0.7, 0.5));

		// Create an overlay, and add the panel
		Ogre::Overlay* overlay = OverlayManager::getSingleton().create("OverlayName");
		overlay->add2D(panel);

		// Add the text area to the panel
		panel->addChild(mTextArea);
		
		// Show the overlay
		overlay->show();
	}
};

// ----------------------------------------------------------------------------
// Main function, just boots the application object
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    // Create application object
    SampleApp app;

    try
    {
        app.go();
    }
    catch ( Ogre::Exception& e )
    {
        std::cerr << "An exception has occured: " << e.getFullDescription();
    }

    return 0;
}
