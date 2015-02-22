/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _hrengin_Model_
#define _hrengin_Model_

#include <vector>
#include <hrengin/core/models.h>

namespace hrengin {

class Model {
public:
	std::vector<Primitive> primitives;
};

} // namespace hrengin
#endif//_hrengin_Model_