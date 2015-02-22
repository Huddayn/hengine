/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#include <hrengin/io/ReadFile.h>

#include "BufferedStream.h"

namespace hrengin {
namespace io {
BufferedStream* createBufferedStream(ReadFile& source)
{
	// temporary hack
	return new BufferedStream(&source);
}

BufferedStream::BufferedStream(ReadFile* source)
: pos_(0), source_(source)
{
	//TODO: may crash on file of zero length
	source_->read(&cur_, 1);
}

BufferedStream::~BufferedStream()
{
//	delete[] buffer_;
}

bool BufferedStream::getCurrent(u8& c)
{
	if(pos_ > source_->getSize()) {
		c = 0;
		return false;
	}

	c = cur_;
	return true;
}

bool BufferedStream::getNext(u8& c)
{
	pos_++;

	if(pos_ > source_->getSize()) {
		c = 0;
		return false;
	}

	source_->read(&cur_, 1);

	c = cur_;

	return true;
}

u32 BufferedStream::getPos() const 
{
	return pos_;
}

} //namespace io
} //namespace hrengin
