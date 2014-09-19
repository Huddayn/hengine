/**
   Copyright (C) 2014  absurdworlds

   License LGPLv3-only:
   GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
   This is free software: you are free to change and redistribute it.
   There is NO WARRANTY, to the extent permitted by law.
 */

#include "CCollisionPhantom.h"
#include "hrToBullet.h"

namespace hrengin {
namespace physics {

CCollisionPhantom::CCollisionPhantom(btCollisionObject* object)
	: details_(object)
{
	details_.obj->setUserPointer(this);
}

void CCollisionPhantom::setPosition(Vector3d<f32> pos) 
{
	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin(btVector3(pos.X,pos.Y,pos.Z));
	localTransform.setRotation(details_.obj->getWorldTransform().getRotation());
	
	details_.obj->setWorldTransform(localTransform);
};

void CCollisionPhantom::setRotation(Vector3d<f32> rot) 
{
	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin(details_.obj->getWorldTransform().getOrigin());
	localTransform.setRotation(btQuaternion(
		rot.Y*math::DEGTORAD64, 
		rot.X*math::DEGTORAD64,
		rot.Z*math::DEGTORAD64));
	
	details_.obj->setWorldTransform(localTransform);
};

Vector3d<f32> CCollisionPhantom::getPosition() const
{
	btVector3 pos = details_.obj->getWorldTransform().getOrigin();

	return Vector3d<f32>(pos.getX(),pos.getY(),pos.getZ());
};

Vector3d<f32> CCollisionPhantom::getRotation() const
{
	btQuaternion rot = details_.obj->getWorldTransform().getRotation();
	
	btVector3 pos;
	quaternionToEuler(rot,pos);

	return Vector3d<f32>(pos.getX(),pos.getY(),pos.getZ());
};

} // namespace physics
} // namespace hrengin
