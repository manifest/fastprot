// ------------------------------------------------------------------
// Copyright 2013 by Andrei Nesterov (ae.nesterov@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ------------------------------------------------------------------

#include "termbuilder.h"
#include <sstream>
#include <cassert>

namespace Fastprot { namespace Detail {
		
TermBuilder::TermBuilder() 
: TermBuilder(new TermBuilder::StandardMode) {
}

TermBuilder::TermBuilder(TermBuilder::Mode *mode)
: TermBuilder(nullptr, std::shared_ptr<TermBuilder::Mode>(mode), std::string(), std::string()) {
}

TermBuilder::TermBuilder(ErlNifEnv *env, const std::shared_ptr<TermBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs)
: TermBuilder(env, mode, appType, appTypeNs, QuickFAST::Messages::FieldIdentityCPtr(), 0) {
}

TermBuilder::TermBuilder(ErlNifEnv *env, const std::shared_ptr<TermBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) 
: Builder(env, appType, appTypeNs), mode_(mode), lengthIdentity_(lengthIdentity), length_(length) {
	data_ = enif_make_list(env, 0);
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int64 value) {
	add(mode().makeProperty(env(), identity, enif_make_int64(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint64 value) {
	add(mode().makeProperty(env(), identity, enif_make_uint64(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int32 value) {
	add(mode().makeProperty(env(), identity, enif_make_int(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint32 value) {
	add(mode().makeProperty(env(), identity, enif_make_uint(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int16 value) {
	add(mode().makeProperty(env(), identity, enif_make_int(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint16 value) {
	add(mode().makeProperty(env(), identity, enif_make_uint(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int8 value) {
	add(mode().makeProperty(env(), identity, enif_make_int(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uchar value) {
	add(mode().makeProperty(env(), identity, enif_make_uint(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::Decimal& value) {
	add(mode().makeProperty(env(), identity, enif_make_double(env(), value)));
}

void TermBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const unsigned char *value, size_t length) {
	add(mode().makeProperty(env(), identity, enif::make_binary(env(), value, length)));
}

QuickFAST::Messages::ValueMessageBuilder &TermBuilder::startMessage(const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new TermBuilder(env(), mode_, appType, appTypeNs));
	return *child_;
}

bool TermBuilder::endMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	data_ = makeDone(mode().makeMessage(env(), child_->result()));
	child_.reset();
	return true;
}

bool TermBuilder::ignoreMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	return true;
}

QuickFAST::Messages::ValueMessageBuilder &TermBuilder::startSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t fieldCount, QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) {
	child_.reset(new TermBuilder(env(), mode_, appType, appTypeNs, lengthIdentity, length));
	return *child_;
}

void TermBuilder::endSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeSequenceProperty(env(), identity, child_->result(), child_->lengthIdentity_, child_->length_));
	child_.reset();
}

QuickFAST::Messages::ValueMessageBuilder &TermBuilder::startSequenceEntry(const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new TermBuilder(env(), mode_, appType, appTypeNs));
	return *child_;
}

void TermBuilder::endSequenceEntry(QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeSequenceEntryProperty(env(), child_->result()));
	child_.reset();
}

QuickFAST::Messages::ValueMessageBuilder &TermBuilder::startGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new TermBuilder(env(), mode_, appType, appTypeNs));
	return *child_;
}

void TermBuilder::endGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeGroupProperty(env(), identity, child_->result()));
	child_.reset();
}

bool TermBuilder::wantLog(unsigned short level) {
	return false;
}

bool TermBuilder::logMessage(unsigned short level, const std::string &reason) {
	return true;
}

bool TermBuilder::reportDecodingError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}

bool TermBuilder::reportCommunicationError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}
		
void TermBuilder::add(ERL_NIF_TERM property) {
	data_ = enif_make_list_cell(env(), property, data_);
}

ERL_NIF_TERM TermBuilder::result() const {
	return data_;
}
		
const TermBuilder::Mode &TermBuilder::mode() const {
	return *mode_;
}

//// class TermBuilder::StandardMode
ERL_NIF_TERM TermBuilder::StandardMode::makeId(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity) const {
	assert(env);

	try {
		return enif_make_uint(env, identity->id().empty() ? 0 : std::stoi(identity->id()));
	}
	catch(std::invalid_argument &) {
		std::stringstream strm;
		strm << "field identifier is not numeric (";
		identity->display(strm);
		strm << ").";
		throw QuickFAST::UsageError("Logic Error", strm.str().c_str());
	}
	catch(std::out_of_range &) {
		std::stringstream strm;
		strm << "converted field identifier would fall out of the range of the integer type (";
		identity->display(strm);
		strm << ").";
		throw QuickFAST::UsageError("Logic Error", strm.str().c_str());
	}
	catch(...) {
		std::stringstream strm;
		strm << "something went wrong when converting field identifier (";
		identity->display(strm);
		strm << ").";
		throw QuickFAST::UsageError("Logic Error", strm.str().c_str());
	}
}
		
ERL_NIF_TERM TermBuilder::StandardMode::makeMessage(ErlNifEnv *env, ERL_NIF_TERM value) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Message must be a list type.");

	return list;
}

ERL_NIF_TERM TermBuilder::StandardMode::makeProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const {
	assert(env);

	// {id, value}
	return enif_make_tuple2(env, 
		makeId(env, identity), 
		value
	);
}
		
ERL_NIF_TERM TermBuilder::StandardMode::makeSequenceProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Sequence must be a list type.");

	// {id, value}
	return enif_make_tuple2(env,
		makeId(env, lengthIdentity),
		list
	);
}

ERL_NIF_TERM TermBuilder::StandardMode::makeSequenceEntryProperty(ErlNifEnv *env, ERL_NIF_TERM value) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Sequence entry must be a list type.");

	return list;
}

ERL_NIF_TERM TermBuilder::StandardMode::makeGroupProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Group must be a list type.");

	// {id, value}
	return enif_make_tuple2(env,
		makeId(env, identity),
		list
	);
}

//// class TermBuilder::ExtendedMode
ERL_NIF_TERM TermBuilder::ExtendedMode::makeId(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity) const {
	assert(env);
	
	return enif_make_tuple3(env,
		enif_make_atom(env, "identity"),
		enif::make_binary_string(env, identity->id()),
		enif::make_binary_string(env, identity->name())
	);
}

ERL_NIF_TERM TermBuilder::ExtendedMode::makeMessage(ErlNifEnv *env, ERL_NIF_TERM value) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Message must be a list type.");

	return list;
}

ERL_NIF_TERM TermBuilder::ExtendedMode::makeProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const {
	assert(env);

	// #value {id, name, value}
	return enif_make_tuple3(env, 
		enif_make_atom(env, "field"),
		makeId(env, identity), 
		value
	);
}
		
ERL_NIF_TERM TermBuilder::ExtendedMode::makeSequenceProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const {
	assert(env);

	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Sequence must be a list type.");

	// #sequence {identity {}, length_identity {}, value}
	return enif_make_tuple4(env,
		enif_make_atom(env, "sequence"),
		makeId(env, identity),
		list,
		enif_make_tuple3(env,
			enif_make_atom(env, "length"),
			makeId(env, lengthIdentity),
			enif_make_uint64(env, length)
		)
	);
}

ERL_NIF_TERM TermBuilder::ExtendedMode::makeSequenceEntryProperty(ErlNifEnv *env, ERL_NIF_TERM value) const {
	assert(env);

	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Sequence entry must be a list type.");

	return list;
}

ERL_NIF_TERM TermBuilder::ExtendedMode::makeGroupProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const {
	assert(env);
	
	ERL_NIF_TERM list;
	if(!enif_make_reverse_list(env, value, &list))
		throw QuickFAST::UsageError("Internal Error", "Group must be a list type.");

	// #group {value}
	return enif_make_tuple3(env,
		enif_make_atom(env, "group"),
		makeId(env, identity),
		value
	);
}

} } // namespace Fastprot::Detail

