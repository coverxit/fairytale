/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  BtOgre test application, main file.
 *
 *        Version:  1.0
 *        Created:  01/14/2009 05:48:31 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "engine-pch.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
#include "engine-application.h"
#include "engine-input.h"

using namespace Ogre;
using namespace fairytale;

/*
 * =====================================================================================
 *    Namespace:  Globals
 *  Description:  A dirty 'globals' hack.
 * =====================================================================================
 */

OgreBites::SdkCameraMan*	debugCameraMan;

namespace Globals
{
    btDynamicsWorld *phyWorld;
    BtOgre::DebugDrawer *dbgdraw;
}

/*
 * =====================================================================================
 *        Class:  BtOgreTestFrameListener
 *  Description:  Derives from ExampleFrameListener and overrides stuf.
 * =====================================================================================
 */

class BtOgreTestFrameListener : public FrameListener
{
	bool frameStarted(const FrameEvent &evt)
	{
	    //Update Bullet world. Don't forget the debugDrawWorld() part!
	    Globals::phyWorld->stepSimulation(evt.timeSinceLastFrame, 10);
	    Globals::phyWorld->debugDrawWorld();

	    //Shows debug if F3 key down.
	    Globals::dbgdraw->setDebugMode(CoreMembers::getInstancePtr()->keyboard->isKeyDown(OIS::KC_F3));
	    Globals::dbgdraw->step();

	    return true;
	}
};

/*
 * =====================================================================================
 *        Class:  BtOgreTestApplication
 *  Description:  Derives from ExampleApplication and overrides stuff.
 * =====================================================================================
 */

class BtOgreTestApplication
{
    protected:
	btAxisSweep3 *mBroadphase;
	btDefaultCollisionConfiguration *mCollisionConfig;
	btCollisionDispatcher *mDispatcher;
	btSequentialImpulseConstraintSolver *mSolver;

	Ogre::SceneNode *mNinjaNode;
	Ogre::Entity *mNinjaEntity;
	btRigidBody *mNinjaBody;
	btCollisionShape *mNinjaShape;

	Ogre::Entity *mGroundEntity;
	btRigidBody *mGroundBody;
	btBvhTriangleMeshShape *mGroundShape;

	Camera* mCamera;
	SceneManager* mSceneMgr;

    public:
	BtOgreTestApplication()
	{
		mSceneMgr = CoreMembers::getInstancePtr()->ogreRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
		mCamera = mSceneMgr->createCamera("PlayerCam");
		mCamera->setFarClipDistance(30000);
		mCamera->setNearClipDistance(5);
		//Some normal stuff.
		mSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));
		mCamera->setPosition(Vector3(10,10,10));
		mCamera->lookAt(Vector3::ZERO);
		mCamera->setAspectRatio(
			Real(CoreMembers::getInstancePtr()->defaultViewport->getActualWidth()) / Real(CoreMembers::getInstancePtr()->defaultViewport->getActualHeight()));

		CoreMembers::getInstancePtr()->defaultViewport->setCamera(mCamera);

	    //Bullet initialisation.
	    mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
	    mCollisionConfig = new btDefaultCollisionConfiguration();
	    mDispatcher = new btCollisionDispatcher(mCollisionConfig);
	    mSolver = new btSequentialImpulseConstraintSolver();

	    Globals::phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
	    Globals::phyWorld->setGravity(btVector3(0,-9.8,0));
	}

	~BtOgreTestApplication()
	{
            //Free rigid bodies
            Globals::phyWorld->removeRigidBody(mNinjaBody);
            delete mNinjaBody->getMotionState();
            delete mNinjaBody;
            delete mNinjaShape;

            Globals::phyWorld->removeRigidBody(mGroundBody);
            delete mGroundBody->getMotionState();
            delete mGroundBody;
            delete mGroundShape->getMeshInterface();
            delete mGroundShape;

	    //Free Bullet stuff.
            delete Globals::dbgdraw;
            delete Globals::phyWorld;

	    delete mSolver;
	    delete mDispatcher;
	    delete mCollisionConfig;
	    delete mBroadphase;
	}

 
	void createScene(void)
	{
		debugCameraMan = new OgreBites::SdkCameraMan(mCamera);
		debugCameraMan->setTopSpeed(5);

	    //----------------------------------------------------------
	    // Debug drawing!
	    //----------------------------------------------------------

	    Globals::dbgdraw = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), Globals::phyWorld);
	    Globals::phyWorld->setDebugDrawer(Globals::dbgdraw);

	    //----------------------------------------------------------
	    // Ninja!
	    //----------------------------------------------------------

	    Vector3 pos = Vector3(0,10,0);
	    Quaternion rot = Quaternion::IDENTITY;

	    //Create Ogre stuff.

	    mNinjaEntity = mSceneMgr->createEntity("ninjaEntity", "Player.mesh");
	    mNinjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ninjaSceneNode", pos, rot);
	    mNinjaNode->attachObject(mNinjaEntity);

	    //Create shape.
	    BtOgre::StaticMeshToShapeConverter converter(mNinjaEntity);
	    mNinjaShape = converter.createSphere();

	    //Calculate inertia.
	    btScalar mass = 5;
	    btVector3 inertia;
	    mNinjaShape->calculateLocalInertia(mass, inertia);

	    //Create BtOgre MotionState (connects Ogre and Bullet).
	    BtOgre::RigidBodyState *ninjaState = new BtOgre::RigidBodyState(mNinjaNode);

	    //Create the Body.
	    mNinjaBody = new btRigidBody(mass, ninjaState, mNinjaShape, inertia);
	    Globals::phyWorld->addRigidBody(mNinjaBody);

	    //----------------------------------------------------------
	    // Ground!
	    //----------------------------------------------------------

	    //Create Ogre stuff.
	    //MeshManager::getSingleton().createPlane("groundPlane", "General", Plane(Vector3::UNIT_Y, 0), 100, 100, 
	    //10, 10, true, 1, 5, 5, Vector3::UNIT_Z);
	    mGroundEntity = mSceneMgr->createEntity("groundEntity", "TestLevel_b0.mesh");
	    //mGroundEntity->setMaterialName("Examples/Rockwall");
	    mSceneMgr->getRootSceneNode()->createChildSceneNode("groundNode")->attachObject(mGroundEntity);

	    //Create the ground shape.
	    BtOgre::StaticMeshToShapeConverter converter2(mGroundEntity);
	    mGroundShape = converter2.createTrimesh();

	    //Create MotionState (no need for BtOgre here, you can use it if you want to though).
	    btDefaultMotionState* groundState = new btDefaultMotionState(
		    btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));

	    //Create the Body.
	    mGroundBody = new btRigidBody(0, groundState, mGroundShape, btVector3(0,0,0));
	    Globals::phyWorld->addRigidBody(mGroundBody);
	}
};

void doBulletTest()
{
	static BtOgreTestApplication app;
	app.createScene();
	CoreMembers::getInstancePtr()->ogreRoot->addFrameListener(new BtOgreTestFrameListener());
}
