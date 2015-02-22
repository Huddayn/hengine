/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_CDebugDrawer_
#define _hrengin_CDebugDrawer_

#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>

#include <hrengin/common/types.h>

#include <hrengin/physics/IDebugDrawer.h>
#include "CPhysicsWorld.h"

namespace hrengin {
namespace physics {


class DebugDraw : public btIDebugDraw {
public:
	DebugDraw(graphics::IRenderingDevice* renderer);

	void drawLine(const btVector3& from, const btVector3& to,
		const btVector3& color);

	void drawContactPoint(const btVector3& PointOnB,
		const btVector3& normalOnB, btScalar distance, int lifeTime,
		const btVector3& color);

	void reportErrorWarning(const char* text);

	void draw3dText(const btVector3& location, const char* text) { }

	void setDebugMode(int mode);
	int getDebugMode() const;

private:
	DebugDraw(const DebugDraw& other) = delete;

	int mode;
	graphics::IRenderingDevice * vmgr_;
};

class IDebugDrawer::Details {
public:
	Details(graphics::IRenderingDevice* renderer)
		//: drawer(renderer)
	{
		debugDraw = new DebugDraw(renderer);
		// temporary
		debugDraw->setDebugMode(
			btIDebugDraw::DBG_DrawWireframe |
			btIDebugDraw::DBG_DrawAabb |
			btIDebugDraw::DBG_DrawContactPoints |
			//btIDebugDraw::DBG_DrawText |
			//btIDebugDraw::DBG_DrawConstraintLimits |
			btIDebugDraw::DBG_DrawConstraints //|
		);
	}
	btIDebugDraw* debugDraw;
};

class CDebugDrawer : public IDebugDrawer {
public:
	CDebugDrawer(graphics::IRenderingDevice* renderer)
		: details_(renderer)
	{
		//details_.drawer = new DebugDraw(renderer);
	}
	virtual void render()
	{
		// temporary, until tendering tools are implemented
		world_->debugDrawWorld();
	}

	virtual void setWorld(IPhysicsWorld* world)
	{
		world_ = world->getDetails()->world;
		world_->setDebugDrawer(details_.debugDraw);
	}

	// for internal use only
	virtual IDebugDrawer::Details* getDetails()
	{
		return &details_;
	}
private:
	IDebugDrawer::Details details_;
	btDynamicsWorld* world_;
};


} // namespace physics
} // namespace hrengin
#endif//_hrengin_CDebugDrawer_