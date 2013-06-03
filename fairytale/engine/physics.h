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

#ifndef __FAIRYTALE_GAME_PHYSICS_H__
#define __FAIRYTALE_GAME_PHYSICS_H__

#include <boost/function.hpp>

class btRigidBody;
class btDynamicsWorld;

namespace fairytale { namespace engine {

	class PhysicsWorld
	{
	private:
		struct PhysicsWorldImpl;
		PhysicsWorldImpl* _mImpl;

	public:
		PhysicsWorld();
		~PhysicsWorld();

		void calculateOnePhysicsFrame(float timeSinceLastFrame);

		btDynamicsWorld* getPhysicsWorld();
	};

} }

#endif
