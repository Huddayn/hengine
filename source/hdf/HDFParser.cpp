/*
 * Copyright (C) 2014  absurdworlds
 *
 * License LGPLv3-only:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#include <stdio.h>

#include <hrengin/common/types.h>
#include <hrengin/io/IBufferedStream.h>
#include <hrengin/hdf/Type.h>

#include "CHDFParser.h"

namespace hrengin {
namespace hdf {

inline bool in(char c, char c1, char c2, char c3, char c4)
{
	return c == c1  ||  c == c2  ||  c == c3  ||  c == c4;
}

inline bool in(char c, char c1, char c2, char c3, char c4, char c5)
{
	return c == c1  ||  c == c2  ||  c == c3  ||  c == c4  ||  c == c5;
}

inline bool isDigit(char c) {
	return (c >= '0' && c <= '9');
}

inline bool isNameBeginChar(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool isNameChar(char c) {
	return isNameBeginChar(c) || isDigit(c) || c == '-' || c == '_';
}

inline bool isWhitespace(char c) {
	return (in(c, ' ', '\t', '\r', '\n'));
}

inline bool isInlineWhitespace(char c) {
	return c == ' ' || c == '\t';
}

hdf::Type hdfTokenToType(const HdfToken& token) 
{
	if(token.value == "bool" || token.value == "b") {
		return Type::Boolean;
	} else if(token.value == "int" || token.value == "i") {
		return Type::Integer;
	} else if(token.value == "float" || token.value == "f") {
		return Type::Float;
	} else if(token.value == "vec3" || token.value == "v3") {
		return Type::Vector3d;
	} else if(token.value == "vec2" || token.value == "v2") {
		return Type::Vector2d;
	} else if(token.value == "string" || token.value == "s") {
		return Type::String;
	} else {
		return Type::Unknown;
	}
}

hdf::Type hdfConvertImpicitType(const HdfToken& token) 
{
	char c = token.value.c_str()[0];
	if(isNameBeginChar(c) || c == '"') {
		return Type::String;
	} else if(c == '-' || (c > '0' && c < '9')) {
		return Type::Float;
	} else {
		return Type::Unknown;
	}
}

IHDFParser* createHDFParser(io::ICharacterStream* stream)
{
	return new CHDFParser(stream);
}

CHDFParser::CHDFParser(io::ICharacterStream* stream)
	: depth_(0), state_(HDF_S_IDLE), stream_(stream)
{
}

CHDFParser::~CHDFParser()
{
}

bool CHDFParser::read() {
	char c;

	stream_->getCurrent(c);

	if(c == 0) {
		return false;
	}

	fastForward();

	stream_->getCurrent(c);

	if(c == 0) {
		return false;
	}

	if(depth_ == 0) {
		while(c == '!') {
			processCommand();
			fastForward();
			stream_->getCurrent(c);
		}
		if(c == '[') {
			state_ = HDF_S_OBJECT;
		}
	} else {
		if(isNameBeginChar(c) || c == '[' || c == ']') {
			state_ = HDF_S_OBJECT;
		}
	}

	return (state_ == HDF_S_PANIC) ? false : true;
}

HdfObjectType CHDFParser::getObjectType()
{
	if(state_ == HDF_S_PANIC) {
		return HDF_OBJ_NULL;
	}

	char c;

	stream_->getCurrent(c);

	if(state_ != HDF_S_OBJECT) {
		error(HDF_LOG_ERROR, "there is no object");
		return HDF_OBJ_NULL;
	} else {
		if(c == '[') {
			// step forward - getObjectName expects a nameChar to
			// be the first char
			stream_->getNext(c); 
			state_ = HDF_S_NODE_BEGIN;
			depth_ ++;
			return HDF_OBJ_NODE;
		} else if (isNameBeginChar(c)) {
			if(depth_ == 0) {
				error(HDF_LOG_ERROR, "unexpected name token");
				return HDF_OBJ_NULL;
			}
			state_ = HDF_S_VALUE_BEGIN;
			return HDF_OBJ_VAL;
		} else if(c == ']') {
			stream_->getNext(c); 
			depth_--;
			state_ = HDF_S_IDLE;
			return HDF_OBJ_NODE_END;

			/*if(state_ != HDF_S_IDLE) {
				error(HDF_LOG_ERROR, "unexpected node-end");
			} else {
				depth_--;
			}*/
		} else if (c == '!') {
			//if(depth_ > 0) {
			error(HDF_LOG_ERROR, "unexpected token: '!'");
			return HDF_OBJ_NULL;
			//}
			//state_ = HDF_S_CMD_BEGIN;
			//return HDF_OBJ_CMD;
		} else {
			std::string msg("invalid character: ");
			msg += c;
			error(HDF_LOG_ERROR, msg);
			return HDF_OBJ_NULL;
		}
	}
}

void CHDFParser::getObjectName(std::string& name)
{
	if(state_ == HDF_S_PANIC) {
		return;
	}

	if(state_ == HDF_S_NODE_BEGIN) {
		readName(name);
		state_ = HDF_S_IDLE;
	} else if(state_ == HDF_S_VALUE_BEGIN) {
		readValueName(name);
		state_ = HDF_S_VALUE_DATA;
	} else {
		error(HDF_LOG_ERROR, "must be called after getObjectType()");
	}
}

void CHDFParser::readFloat(float& val)
{
	readValue<f32>(val);
}
void CHDFParser::readFloat(double& val)
{
	readValue<f64>(val);
}
void CHDFParser::readInt(u32& val)
{
	readValue<u32>(val);
}
void CHDFParser::readInt(i32& val)
{
	readValue<i32>(val);
}
void CHDFParser::readBool(bool& val)
{
	readValue<bool>(val);
}
void CHDFParser::readString(std::string& val)
{
	readValue<std::string>(val);
}
void CHDFParser::readVector3d(Vector3d<f32>& val)
{
	readValue<Vector3d<f32>>(val);
}

//TODO: rewrite those two properly
void CHDFParser::skipValue() 
{
	HdfToken token;
	hdf::Type type;

	bool hasType = parseType(token);

	if(hasType) {
		type = hdfTokenToType(token);
		readToken(token);
	} else {
		type = hdfConvertImpicitType(token);
	}

	if(type == Type::Vector3d || type == Type::Vector2d) {
		readToken(token);
		if(type == Type::Vector3d) {
			readToken(token);
		}
	}
}

void CHDFParser::skipNode() 
{
	char c;

	stream_->getCurrent(c);

	u32 depth = 1;

	while (depth > 0) {
		stream_->getNext(c);

		if(c == '[' ) {
			depth++;
		} else if(c == ']') {
			depth--;
		}
	}

	read();
}

void CHDFParser::error(hdf::ParserMessage type, std::string msg)
{
	errors_.push_back(msg);
	printf("[HDF:%u]: %s\n",stream_->getPos(),msg.c_str());

	if(type == HDF_LOG_ERROR) {
		state_ = HDF_S_PANIC;
	}
}


//void CHDFParser::skip(bool (*condition)(char))
template<bool (*condition)(char)>
void CHDFParser::skip()
{
	char c;

	stream_->getCurrent(c);

	while(condition(c) && c != 0) {
		stream_->getNext(c);
	}
}

inline bool notLineBreak(char c)
{
	return c != '\n';
}

void CHDFParser::skipLine()
{
	skip<notLineBreak>();
}

void CHDFParser::skipWhitespace()
{
	skip<isWhitespace>();
}

void CHDFParser::skipInlineWhitespace()
{
	skip<isInlineWhitespace>();
}

void CHDFParser::fastForward() {
	char c;

	stream_->getCurrent(c);

	bool needsFastForward = isWhitespace(c) || c == '/';

	while (needsFastForward) {
		if(isWhitespace(c)) {
			skipWhitespace();
		} else if(c == '/') {
			stream_->getNext(c);

			if(c == '/') {
				//token.type = tokenCOMMENT;
				skipLine();
			} else {
				error(HDF_LOG_WARNING,"unexpected token: /");
			}
		}		

		stream_->getCurrent(c);
		needsFastForward = isWhitespace(c) || c == '/';
		//stream_->getNext(c);
	}
}

bool CHDFParser::parseType(HdfToken& token) {
	skipInlineWhitespace();

	char c;

	stream_->getCurrent(c);

	if(c == '=') {
		stream_->getNext(c);
	} else {
		error(HDF_LOG_ERROR, "illegal token, expected '='");
	}

	skipInlineWhitespace();

	stream_->getCurrent(c);

	if(isNameBeginChar(c)) {
		token.type = HDF_TOKEN_NAME;
		readName(token.value, ':');
	} else {
		error(HDF_LOG_ERROR, "illegal token, expected typename");	
	}

	skipInlineWhitespace();

	stream_->getCurrent(c);

	if(c == ':') {
		stream_->getNext(c);
		return true;
	} else {
		//stream_->getNext(c);
		return false;
	}
}

void CHDFParser::readToken(HdfToken& token)
{
	fastForward();

	char c;

	stream_->getCurrent(c);

	if(isNameBeginChar(c)) {
		token.type = HDF_TOKEN_NAME;
		readName(token.value);	
	} else if(c == '-' || (c >= '0' && c <= '9')) {
		token.type = HDF_TOKEN_NUMBER;
		readNumber(token.value);
	} else if(c == '"') {
		token.type = HDF_TOKEN_STRING;
		readStringToken(token.value);
	} else {
		error(HDF_LOG_ERROR,"illegal token");
	}
}

void CHDFParser::readStringToken(std::string& val) {
	val = "";
	char c;

	stream_->getCurrent(c);
#if 0
	if(c != '"') {
		// should not get this error
		error(HDF_LOG_ERROR, "illegal string token");
	}
#endif
	stream_->getNext(c);

	while (c != '"') {
		if ( c == '\\' ) {
			stream_->getNext(c);
		}
		val += c;
		stream_->getNext(c);
	}

	stream_->getNext(c);
}

void CHDFParser::readNumber(std::string& val)
{
	val = "";
	char c;

	stream_->getCurrent(c);

	while (!isWhitespace(c) && (c != ']')) {
		if (!(c >= '0' && c <= '9') && !in(c, '.', 'e', 'E', '+', '-' )) {
			error(HDF_LOG_WARNING, "invalid number");
		}
		val += c;
		stream_->getNext(c);
	}
}

void CHDFParser::readName(std::string& name, char stop)
{
	name = "";
	char c;

	stream_->getCurrent(c);

	while(!isWhitespace(c) && (c != stop) && (c != ']')) {
		if(isNameChar(c)) {
			name += c;
		} else {
			error(HDF_LOG_WARNING, "invalid name char");
		}
		stream_->getNext(c);
	}
}

void CHDFParser::readValueName(std::string& name)
{
	readName(name, '=');
}

void CHDFParser::readTypeName(std::string& name)
{
	readName(name, ':');
}


template<typename T> 
void CHDFParser::readValue(T& var)
{
	HdfToken token;
	hdf::Type type;

	bool hasType = parseType(token);

	if(hasType) {
		type = hdfTokenToType(token);
		readToken(token);
	} else {
		type = hdfConvertImpicitType(token);
	}

	if(checkType<T>(type) == false) {
		error(HDF_LOG_ERROR, "type mismatch: " + token.value);
		//skipValue(type);

		return;
	}

	convertValue<T>(token, var);
	state_ = HDF_S_IDLE;
}

// TODO: make helper class to reduce almost duplicate functions

template<typename T>
void CHDFParser::convertValue(HdfToken& token, T& val)
{
	// should never get this error
	error(HDF_LOG_ERROR, "unknown type");
}

template<>
void CHDFParser::convertValue(HdfToken& token, f32& val)
{	
	val = strtof(token.value.c_str(), 0);
}

template<>
void CHDFParser::convertValue(HdfToken& token, f64& val)
{
	val = strtod(token.value.c_str(), 0);
}

template<>
void CHDFParser::convertValue(HdfToken& token, Vector3d<f32>& val)
{
	val[0] = strtod(token.value.c_str(), 0);

	readToken(token);

	val[1] = strtod(token.value.c_str(), 0);

	readToken(token);

	val[2] = strtod(token.value.c_str(), 0);
}

template<>
void CHDFParser::convertValue(HdfToken& token, std::string& val)
{	
	val = token.value;
}

template<>
void CHDFParser::convertValue(HdfToken& token, u32& val)
{	
	val = strtoul(token.value.c_str(), 0, 10);
}

template<>
void CHDFParser::convertValue(HdfToken& token, i32& val)
{	
	val = strtol(token.value.c_str(), 0, 10);
}

template<>
void CHDFParser::convertValue(HdfToken& token, bool& val)
{
	if(token.value == "true" || token.value == "1") {
		val = true;
	} else if(token.value == "false" || token.value == "0") {
		val = false;
	} else {
		error(HDF_LOG_WARNING, "invalid boolean value");
	}
}


// todo: rewrite
void CHDFParser::processCommand() {
	HdfToken token;

	char c;

	stream_->getCurrent(c);

	if (c == '!') {
		stream_->getNext(c);
	} else {
		error(HDF_LOG_ERROR, "No command to process");
	}

	readToken(token);

	if (token.value == "hndf_version") {
		readToken(token);
		if(token.type != HDF_TOKEN_STRING) {
			error(HDF_LOG_ERROR,"expected string");
			return ;
		} else if(token.value == "1.1") {
			error(HDF_LOG_ERROR, "Version 1.1 is outdated.");
			return;
		} else {
			error(HDF_LOG_ERROR, "your version is bullshit!");
			return;
		}
	} else if (token.value == "hdf_version") {
		readToken(token);
		if(token.type != HDF_TOKEN_STRING) {
			error(HDF_LOG_ERROR,"expected string");
			return ;
		} else if(token.value == "1.1.1") {
			error(HDF_LOG_NOTICE, "HDF version: 1.1.1");
			return;
		} else if(token.value == "1.1") {
			error(HDF_LOG_ERROR, "Version 1.1 is outdated.");
			return;
		} else {
			error(HDF_LOG_ERROR, "your version is bullshit!");
			return;
		}
	}
}

} // namespace io
} // namespace hrengin