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

#ifndef __FAIRYTALE_GAME_VEHICLE_H__
#define __FAIRYTALE_GAME_VEHICLE_H__

#include "engine-pch.h"

namespace fairytale
{
	///VehicleDemo shows how to setup and use the built-in raycast vehicle
	class Vehicle : public OIS::KeyListener, public Ogre::FrameListener
	{
	public:

		btRigidBody*								m_carChassis;
		btAlignedObjectArray<btCollisionShape*>		m_collisionShapes;

		class btBroadphaseInterface*				m_overlappingPairCache;
		class btCollisionDispatcher*				m_dispatcher;
		class btConstraintSolver*					m_constraintSolver;
		class btDefaultCollisionConfiguration*		m_collisionConfiguration;
		class btTriangleIndexVertexArray*			m_indexVertexArrays;

		btVector3*									m_vertices;

		btRaycastVehicle::btVehicleTuning			m_tuning;
		btVehicleRaycaster*							m_vehicleRayCaster;
		btRaycastVehicle*							m_vehicle;
		btCollisionShape*							m_wheelShape;

		float										m_cameraHeight;
		float										m_minCameraDistance;
		float										m_maxCameraDistance;

		Vehicle();
		virtual ~Vehicle();

		virtual bool keyPressed(const OIS::KeyEvent& evt);
		virtual bool keyReleased(const OIS::KeyEvent& evt);
		virtual void initPhysics();
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	};

}

#endif
