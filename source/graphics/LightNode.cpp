/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#include <irrlicht/irrlicht.h>

#include <awrts/graphics/VideoManager.h>

#include "VideoManager.h"

#include "LightNode.h"
#include "IrrDummyNode.h"

namespace awrts {
namespace scene {

LightNode::LightNode (SceneManager* sceneManager,
	irr::scene::ILightSceneNode* lightNode)
	: node_(lightNode)
{
	dummy_ = new IrrDummyNode(this, node_->getSceneManager());
	dummy_->addChild(node_);
}

LightNode::~LightNode ()
{
	node_->remove();
	dummy_->remove();
}

} // namespace graphics
} // namespace awrts