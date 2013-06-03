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

#include "pch.h"

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lockfree/queue.hpp>

#include "physics.h"
#include "btogre-fork/BtOgreExtras.h"
#include "btogre-fork/BtOgreGP.h"
#include "btogre-fork/BtOgrePG.h"

namespace fairytale { namespace engine {

	class btDiscreteDynamicsWorldDelayed : public btDiscreteDynamicsWorld
	{

	};

	struct PhysicsWorld::PhysicsWorldImpl
	{
		boost::scoped_ptr<btAxisSweep3>							broadPhase;
		boost::scoped_ptr<btDefaultCollisionConfiguration>		collisionConfig;
		boost::scoped_ptr<btCollisionDispatcher>				dispatcher;
		boost::scoped_ptr<btSequentialImpulseConstraintSolver>	solver;
		boost::scoped_ptr<btDynamicsWorld>						phyWorld;

		boost::lockfree::queue<boost::shared_ptr<boost::function<void()>>>	postedOperations;
		
		PhysicsWorldImpl::PhysicsWorldImpl() :
			broadPhase(new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024)),
			collisionConfig(new btDefaultCollisionConfiguration()),
			dispatcher(new btCollisionDispatcher(collisionConfig.get())),
			solver(new btSequentialImpulseConstraintSolver()),
			phyWorld(new btDiscreteDynamicsWorld(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get()))
		{
			phyWorld->setGravity(btVector3(0.0f ,-9.8f, 0.0f));
		}
	};

	PhysicsWorld::PhysicsWorld() : _mImpl(new PhysicsWorldImpl) {}

	PhysicsWorld::~PhysicsWorld()
	{
		delete _mImpl;
	}

	void PhysicsWorld::calculateOnePhysicsFrame(float timeSinceLastFrame)
	{
		boost::shared_ptr<boost::function<void()>> singleOperation;

		while(_mImpl->postedOperations.pop(singleOperation))
			(*singleOperation.get())();

		_mImpl->phyWorld->stepSimulation((btScalar)timeSinceLastFrame);
	}

	btDynamicsWorld* PhysicsWorld::getPhysicsWorld()
	{
		return _mImpl->phyWorld.get();
	}

	void PhysicsWorld::postOperation(const boost::function<void()>& operation)
	{
		_mImpl->postedOperations.push(boost::shared_ptr<boost::function<void()>>(new boost::function<void()>(operation)));
	}

} }
