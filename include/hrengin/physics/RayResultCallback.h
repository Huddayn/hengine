/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_RayResultCallback_
#define _hrengin_RayResultCallback_

#include <hrengin/physics/CollisionObject.h>
#include <hrengin/physics/BroadphaseProxy.h>

namespace hrengin {
namespace physics {
/*!
 * Structure for storing raytest result
 */
struct RaytestResult {
	RaytestResult(const CollisionObject* collisionObject,
		f32 hitFrac, Vector3d<f32> hitPos, Vector3d<f32> hitNorm)
	: collObject(collisionObject),
	hitFraction(hitFrac),
	hitPosition(hitPos),
	hitNormal(hitNorm)
	{
	}

	const CollisionObject* collObject;

	f32             hitFraction;
	Vector3d<f32>   hitNormal;
	Vector3d<f32>   hitPosition;
};

//! Interface for raytest callbacks
class RayResultCallback {
public:
	static bool const singleHit = true;

	f32 closestHitFraction;
	CollisionFilter collFilter;

	virtual bool needsCollision(BroadphaseProxy* proxy)
	{
		bool collides = (proxy->collFilter.group & collFilter.mask) != 0;
		collides = collides && (collFilter.group & proxy->collFilter.mask);
		return collides;
	}

	virtual f32 addResult(RaytestResult& result) = 0;
	virtual bool hasHit() const = 0;
};

//! This class is used to report single raytest result
class ClosestHitCallback : public IRayResultCallback {
public:
	static bool const singleHit = true;

	CollisionObject const* collObject;
	Vector3d<f32> hitPoint;
	Vector3d<f32> hitNormal;

	ClosestHitCallback()
		: collObject(0), hitPoint(0,0,0), hitNormal(0,0,0)
	{

	}

	virtual f32 addResult(RaytestResult& result)
	{
		collObject = result.collObject;
		closestHitFraction = result.hitFraction;
		hitPoint = result.hitPosition;
		hitNormal = result.hitNormal;

		return closestHitFraction;
	}

	virtual bool hasHit() const
	{
		return (collObject != 0);
	}
};

} // namespace graphics
} // namespace hrengin
#endif //_hrengin_RayResultCallback_