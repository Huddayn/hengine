/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3 or later:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef _aw_SettingsManager_
#define _aw_SettingsManager_
#include <string>

#include <aw/common/types.h>

#include <aw/core/core.h>

namespace aw {
namespace core {
/*!
 * Interface to store settings and access them by strng keys
 */
class SettingsManager {
public:
	//! Virtual destructor
	virtual ~SettingsManager ()
	{
	}

	//virtual void registerSetting(std::string key, hdf::Type type) = 0;

	virtual bool getValue(std::string key, std::string & val) = 0;
	virtual bool getValue(std::string key, f64 & val) = 0;
	virtual bool getValue(std::string key, i32 & val) = 0;
	virtual bool getValue(std::string key, bool & val) = 0;

	virtual bool setValue(std::string key, std::string val) = 0;
	virtual bool setValue(std::string key, f64 val) = 0;
	virtual bool setValue(std::string key, i32 val) = 0;
	virtual bool setValue(std::string key, bool val) = 0;
};

/*!
 * Create an instance of settings manager
 */
AW_CORE_EXP SettingsManager* createSettingsManager();

} // namespace core
} // namespace aw
#endif//_aw_SettingsManager_
