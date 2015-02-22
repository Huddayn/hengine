/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_CCameraNode_
#define _hrengin_CCameraNode_

#include <hrengin/graphics/IVideoManager.h>

#include <hrengin/scene/ISceneNode.h>
#include <hrengin/scene/ICameraNode.h>

namespace irr {
class IrrlichtDevice;
namespace scene {
class ICameraSceneNode;
class ISceneManager;
}
}
namespace hrengin {
namespace scene {
class CSceneManager;
class CIrrDummyNode;

class CCameraNode : public ICameraNode {
public:
	CCameraNode(CSceneManager* sceneManager,
		irr::scene::ICameraSceneNode* camNode,
		irr::scene::ISceneManager* irrScMgr,
		irr::IrrlichtDevice* device);
	virtual ~CCameraNode();

	virtual void setTarget(INode* target);
	virtual void resetTarget();

	virtual void setFOV(f64 fov);
	virtual void setNearPlane(f64 dist);
	virtual void setFarPlane(f64 dist);

	virtual Line3d<f32> getRayFromScreen(i32 x, i32 y);
private:
	irr::IrrlichtDevice* device_;
	irr::scene::ISceneManager* scmgr_;

	CIrrDummyNode* dummy_;
	irr::scene::ICameraSceneNode* camera_;
};

} // namespace scene
} // namespace hrengin
#endif//_hrengin_CCameraNode_