/**
   Copyright (C) 2014  absurdworlds

   License LGPLv3-only:
   GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
   This is free software: you are free to change and redistribute it.
   There is NO WARRANTY, to the extent permitted by law.
 */

#include "CCollisionPhantom.h"
#include "CRigidBody.h"

#include "CPhysicsWorld.h"

namespace hrengin {
namespace physics {

CPhysicsWorld::CPhysicsWorld(btCollisionConfiguration* configuration,
	btBroadphaseInterface* broadphase,
	btCollisionDispatcher* dispatcher,
	btConstraintSolver* constraintSolver)
	: collisionConfiguration_(configuration), broadphase_(broadphase),
	dispatcher_(dispatcher), solver_(constraintSolver)
{
	dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collisionConfiguration_);
	dynamicsWorld_->setGravity(btVector3(0,-10,0));
}

CPhysicsWorld::~CPhysicsWorld()
{
	int i;
	for (i = dynamicsWorld_->getNumCollisionObjects() -1; i >= 0; -- i) {
		btCollisionObject* obj = dynamicsWorld_->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		dynamicsWorld_->removeCollisionObject( obj );
		delete obj;
	}

	delete dynamicsWorld_;
	delete solver_;
	delete broadphase_;
	delete dispatcher_;
	delete collisionConfiguration_;
}

btScalar CPhysicsWorld::getDeltaTime()
{
	btScalar dt = (btScalar)clock_.getTimeMicroseconds();
	clock_.reset();
	return dt;
}


void CPhysicsWorld::addBody(IRigidBody* body)
{
	btRigidBody* rb = static_cast<btRigidBody*>(body->getDetails()->obj);
	dynamicsWorld_->addRigidBody(rb);
	dynamicsWorld_->updateAabbs();
}

void CPhysicsWorld::addBody(IRigidBody* body, CollisionFilter filter)
{
	btRigidBody* rb = static_cast<btRigidBody*>(body->getDetails()->obj);
	dynamicsWorld_->addRigidBody(rb, filter.group, filter.mask);
	dynamicsWorld_->updateAabbs();
}

void CPhysicsWorld::addObject(ICollisionObject* object)
{
	btCollisionObject* obj = object->getDetails()->obj;
	dynamicsWorld_->addCollisionObject(obj);
	//dynamicsWorld_->updateAabbs();
}

void CPhysicsWorld::addObject(ICollisionObject* object, CollisionFilter filter)
{
	btCollisionObject* obj = object->getDetails()->obj;
	dynamicsWorld_->addCollisionObject(obj, filter.group, filter.mask);
	//dynamicsWorld_->updateAabbs();
}

void CPhysicsWorld::removeBody(IRigidBody* body)
{
	btRigidBody* obj = static_cast<btRigidBody*>(body->getDetails()->obj);
	dynamicsWorld_->removeRigidBody(obj);
}

void CPhysicsWorld::removeObject(ICollisionObject* object)
{
	btCollisionObject* obj = object->getDetails()->obj;
	dynamicsWorld_->removeCollisionObject(obj);
}

ICollisionObject* CPhysicsWorld::castRay(Vector3d<f32> from, Vector3d<f32> to, u16 filters)
{
	btVector3 btfrom = btVector3(from.X,from.Y,from.Z);
	btVector3 btto = btVector3(to.X,to.Y,to.Z);
	btCollisionWorld::ClosestRayResultCallback resultCallback(btfrom, btto);
	if(filters) {
		resultCallback.m_collisionFilterGroup = COL_UNIT;
		resultCallback.m_collisionFilterMask = filters;
	}

	dynamicsWorld_->rayTest(btfrom, btto, resultCallback);

	if (resultCallback.hasHit()) {
		return (ICollisionObject*)resultCallback.m_collisionObject->getUserPointer();
	}

	return 0;
}

void CPhysicsWorld::debugDraw()
{
	dynamicsWorld_->debugDrawWorld();
}

void CPhysicsWorld::setDebugDrawer(IDebugDrawer* drawer)
{
	btIDebugDraw* debugDraw = drawer->getDetails()->debugDraw;
	dynamicsWorld_->setDebugDrawer(debugDraw);
}

bool CPhysicsWorld::step()
{
	btScalar ms = getDeltaTime();
	
	if (!dynamicsWorld_) {
		return false;
	}

	static const btScalar subSteps = 1;
	static const btScalar timeStep = 1.0f / 60.0f;

	dynamicsWorld_->stepSimulation(ms / 1000000.0f, subSteps, timeStep);

	return true;
}


} // namespace physics
} // namespace hrengin
