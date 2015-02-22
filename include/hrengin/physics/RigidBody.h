/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_RigidBody_
#define _hrengin_RigidBody_

#include <hrengin/physics/CollisionObject.h>
#include <hrengin/physics/MotionState.h>

namespace hrengin {
namespace physics {
/*!
 * Represents a rigid physical body
 */
class RigidBody : public CollisionObject {
public:
	virtual ~RigidBody() {};

	//! Contains information needed to construct a rigid body
	struct RigidBodyConstructionInfo {
	public:
		//! Mass of the body
		//! If mass is zero, body is static
		f32 mass;

		// 
		CollisionFilter collisionFilterInfo;

		// NYI
		// const CollisionShape shape;

		f32 linearDamping;
		f32 angularDamping;

		// NYI
		// Vector3d<f32> localInertia;

		// 
		// MotionState* motionState;
		Vector3d<f32> position;
		//Vector4d<f32> rotation;

		f32 friction;
		f32 rollingFriction;
		f32 restitution;

		//! Default construction parameters
		RigidBodyConstructionInfo()
			: mass(1.0f),
			linearDamping(0.0f),
			angularDamping(0.0f),
			position(0.0f, 0.0f, 0.0f),
			friction(0.5f),
			rollingFriction(0.0f),
			restitution(0.0f)
		{
		}

		//! Construct specifying mass
		RigidBodyConstructionInfo(f32 mass)
			: mass(mass),
			linearDamping(0.0f),
			angularDamping(0.0f),
			position(0.0f, 0.0f, 0.0f),
			friction(0.5f),
			rollingFriction(0.0f),
			restitution(0.0f)
		{
		}
	};

	//! Set motion state \see MotionState
	virtual void setMotionState(MotionState* motionState) = 0;

	//! Get position of the body
	virtual Vector3d<f32> getPosition() const = 0;

	//! Get orientation of the body
	virtual Vector3d<f32> getRotation() const = 0;
};

} // namespace physics
} // namespace hrengin
#endif//_hrengin_RigidBody_