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

#include "engine-application.h"
#include "engine-input.h"
#include "debug-cameraman.h"

using namespace Ogre;
using namespace fairytale;

/*
 * =====================================================================================
 *    Namespace:  Globals
 *  Description:  A dirty 'globals' hack.
 * =====================================================================================
 */

boost::shared_ptr<fairytale::DebugCameraMan> debugCameraMan;

/*
 * =====================================================================================
 *        Class:  BtOgreTestApplication
 *  Description:  Derives from ExampleApplication and overrides stuff.
 * =====================================================================================
 */
SceneManager* mSceneMgr;

class BtOgreTestApplication
{
    protected:


	Ogre::SceneNode *mNinjaNode;
	Ogre::Entity *mNinjaEntity;
	btRigidBody *mNinjaBody;
	btCollisionShape *mNinjaShape;

	Ogre::Entity *mGroundEntity;
	btRigidBody *mGroundBody;
	btBvhTriangleMeshShape *mGroundShape;

	Camera* mCamera;
	

    public:
	BtOgreTestApplication()
	{
		mSceneMgr = CoreMembers::getInstancePtr()->ogreRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
		mCamera = mSceneMgr->createCamera("PlayerCam");
		mCamera->setFarClipDistance(10000);
		mCamera->setNearClipDistance(0.05);
		//Some normal stuff.
		mSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));
		mCamera->setPosition(Vector3(10,10,10));
		mCamera->lookAt(Vector3::ZERO);
		mCamera->setAspectRatio(
			Real(CoreMembers::getInstancePtr()->defaultViewport->getActualWidth()) / Real(CoreMembers::getInstancePtr()->defaultViewport->getActualHeight()));

		CoreMembers::getInstancePtr()->defaultViewport->setCamera(mCamera);

		debugCameraMan.reset(new DebugCameraMan(mCamera));
		debugCameraMan->setTopSpeed(100);

		{
			LOCK_AND_GET_INSTANCE_PTR(KeyListenerManager, Keyboard);
			Keyboard->registerListener("SDK_CAMERA_MAN", debugCameraMan);
			LOCK_AND_GET_INSTANCE_PTR(MouseListenerManager, Mouse);
			Mouse->registerListener("SDK_CAMERA_MAN", debugCameraMan);
		}

		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);
		core->ogreRoot->addFrameListener(new DebugCameraManFrameListener(debugCameraMan));

		//----------------------------------------------------------
		// Debug drawing!
		//----------------------------------------------------------

		core->dbgDraw.reset(new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), core->phyWorld.get()));
		core->phyWorld->setDebugDrawer(core->dbgDraw.get());
	}

	~BtOgreTestApplication()
	{
		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);
            //Free rigid bodies
            core->phyWorld->removeRigidBody(mNinjaBody);
            delete mNinjaBody->getMotionState();
            delete mNinjaBody;
            delete mNinjaShape;

           core->phyWorld->removeRigidBody(mGroundBody);
            delete mGroundBody->getMotionState();
            delete mGroundBody;
            delete mGroundShape->getMeshInterface();
            delete mGroundShape;
	}

 
	void createScene(void)
	{
		LOCK_AND_GET_INSTANCE_PTR(CoreMembers, core);

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
	    core->phyWorld->addRigidBody(mNinjaBody);

	    //----------------------------------------------------------
	    // Ground!
	    //----------------------------------------------------------

	    //Create Ogre stuff.
	    MeshManager::getSingleton().createPlane("groundPlane", "General", Plane(Vector3::UNIT_Y, 0), 1000, 1000, 
	    10, 10, true, 1, 5, 5, Vector3::UNIT_Z);
	    mGroundEntity = mSceneMgr->createEntity("groundEntity", "groundPlane");
	    mGroundEntity->setMaterialName("TechDemo/Ground");
	    mSceneMgr->getRootSceneNode()->createChildSceneNode("groundNode")->attachObject(mGroundEntity);

	    //Create the ground shape.
	    BtOgre::StaticMeshToShapeConverter converter2(mGroundEntity);
	    mGroundShape = converter2.createTrimesh();

	    //Create MotionState (no need for BtOgre here, you can use it if you want to though).
	    btDefaultMotionState* groundState = new btDefaultMotionState(
		    btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));

	    //Create the Body.
	    mGroundBody = new btRigidBody(0, groundState, mGroundShape, btVector3(0,0,0));
	    core->phyWorld->addRigidBody(mGroundBody);
	}
};

void doBulletTest()
{
	static BtOgreTestApplication app;
	app.createScene();
}
