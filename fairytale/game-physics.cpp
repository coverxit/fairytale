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

#include "game-physics.h"
#include "engine-application.h"

namespace fairytale
{
	class PhysicsFrameListener : public Ogre::FrameListener
	{
		bool frameStarted(const Ogre::FrameEvent &evt)
		{
			GamePhysics::getInstance().phyWorld->stepSimulation(evt.timeSinceLastFrame, 10);

			return true;
		}
	};

	PhysicsFrameListener _physicslistner;

	GamePhysics::GamePhysics() :
		broadPhase(new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024)),
		collisionConfig(new btDefaultCollisionConfiguration()),
		dispatcher(new btCollisionDispatcher(collisionConfig)),
		solver(new btSequentialImpulseConstraintSolver()),
		phyWorld(new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig))
	{
		phyWorld->setGravity(btVector3(0,-9.8,0));

		LOCK_AND_GET_INSTANCE_PTR(Application, app);
		app->ogreRoot->addFrameListener(&_physicslistner);
	}

	GamePhysics::~GamePhysics()
	{
		delete phyWorld;

		delete broadPhase;
		delete collisionConfig;
		delete dispatcher;
		delete solver;
	}
};
