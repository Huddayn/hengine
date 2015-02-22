/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_Sound_
#define _hrengin_Sound_
#include <AL/al.h>

#include <hrengin/common/hrengintypes.h>

#include <hrengin/sound/Sound3D.h>

#include "ALVector.h"

namespace hrengin {
namespace audio {

class Sound : public Sound3D {
public:
	Sound(ALuint bufferHandle);
	~Sound();
	virtual bool play();
	virtual bool stop();
	virtual void move(Vector3d pos, f32 timeStep);
	virtual void setPosition(Vector3d pos);

private:
	void playThreaded();
	bool stopped_;

	ALuint source_;
 
	//ALVector3d pos_;
	//ALVector3d vel_;
	ALfloat pos_[3];
	ALfloat vel_[3];
};

} // namespace audio
} // namespace hrengin
#endif//_hrengin_Sound_
