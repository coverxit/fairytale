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

#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/scoped_ptr.hpp>

#include "physics.h"
#include "btogre-fork/BtOgreExtras.h"
#include "btogre-fork/BtOgreGP.h"
#include "btogre-fork/BtOgrePG.h"

namespace fairytale { namespace engine {

	struct PhysicsWorld::PhysicsWorldImpl
	{
		boost::scoped_ptr<btAxisSweep3>							broadPhase;
		boost::scoped_ptr<btDefaultCollisionConfiguration>		collisionConfig;
		boost::scoped_ptr<btCollisionDispatcher>				dispatcher;
		boost::scoped_ptr<btSequentialImpulseConstraintSolver>	solver;
		boost::scoped_ptr<btDynamicsWorld>						phyWorld;
		boost::mutex											physicsMutex;
		bool													shutdownCalled;
		boost::posix_time::ptime								lastFrameTime;

		PhysicsWorldImpl::PhysicsWorldImpl() :
			broadPhase(new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024)),
			collisionConfig(new btDefaultCollisionConfiguration()),
			dispatcher(new btCollisionDispatcher(collisionConfig.get())),
			solver(new btSequentialImpulseConstraintSolver()),
			phyWorld(new btDiscreteDynamicsWorld(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get())),
			shutdownCalled(false),
			lastFrameTime(boost::posix_time::microsec_clock::local_time())
		{
			phyWorld->setGravity(btVector3(0.0f ,-9.8f, 0.0f));
		}
	};

	PhysicsWorld::PhysicsWorld() : _mImpl(new PhysicsWorldImpl) {}

	PhysicsWorld::~PhysicsWorld()
	{
		delete _mImpl;
	}

	void PhysicsWorld::setGravity(float x, float y, float z)
	{
		boost::lock_guard<boost::mutex>(_mImpl->physicsMutex);
		_mImpl->phyWorld->setGravity(btVector3(x, y, z));
	}

	void PhysicsWorld::startPhysicsSimulation()
	{
		while(!_mImpl->shutdownCalled)
		{
			boost::lock_guard<boost::mutex>(_mImpl->physicsMutex);
			
			boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() - _mImpl->lastFrameTime;
			_mImpl->lastFrameTime = boost::posix_time::microsec_clock::local_time();
			_mImpl->phyWorld->stepSimulation((float)((double)diff.total_microseconds() / (double)1000.0));
		}
	}

	void PhysicsWorld::stopPhysicsSimulation()
	{
		_mImpl->shutdownCalled = true;
	}

	void PhysicsWorld::addRigidBody(btRigidBody* body)
	{
		boost::lock_guard<boost::mutex>(_mImpl->physicsMutex);
		_mImpl->phyWorld->addRigidBody(body);
	}

	void PhysicsWorld::removeRigidBody(btRigidBody* body)
	{
		boost::lock_guard<boost::mutex>(_mImpl->physicsMutex);
		_mImpl->phyWorld->removeRigidBody(body);
	}

} }
