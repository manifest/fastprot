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

#include "jsonbuilder.h"

namespace Fastprot { namespace Detail {
		
JsonBuilder::JsonBuilder() 
: JsonBuilder(new JsonBuilder::StandardMode) {
}

JsonBuilder::JsonBuilder(JsonBuilder::Mode *mode)
: JsonBuilder(std::shared_ptr<JsonBuilder::Mode>(mode), std::string(), std::string()) {
}

JsonBuilder::JsonBuilder(const std::shared_ptr<JsonBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs)
: JsonBuilder(mode, appType, appTypeNs, QuickFAST::Messages::FieldIdentityCPtr(), 0) {
}

JsonBuilder::JsonBuilder(const std::shared_ptr<JsonBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) 
: Builder(nullptr, appType, appTypeNs), mode_(mode), lengthIdentity_(lengthIdentity), length_(length) {
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int64 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint64 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int32 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint32 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int16 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint16 value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int8 value) {
	add(mode().makeProperty(identity, std::to_string(static_cast<std::int16_t>(value))));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uchar value) {
	add(mode().makeProperty(identity, std::to_string(static_cast<std::uint16_t>(value))));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::Decimal& value) {
	add(mode().makeProperty(identity, std::to_string(value)));
}

void JsonBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const unsigned char *value, size_t length) {
	add(mode().makeProperty(identity, std::string(reinterpret_cast<const char *>(value), length)));
}

QuickFAST::Messages::ValueMessageBuilder &JsonBuilder::startMessage(const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new JsonBuilder(mode_, appType, appTypeNs));
	return *child_;
}

bool JsonBuilder::endMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	data_ = makeDone(enif::make_binary_string(env(), mode().makeMessage(child_->data())));
	child_.reset();
	return true;
}

bool JsonBuilder::ignoreMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	return true;
}

QuickFAST::Messages::ValueMessageBuilder &JsonBuilder::startSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t fieldCount, QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) {
	child_.reset(new JsonBuilder(mode_, appType, appTypeNs, lengthIdentity, length));
	return *child_;
}

void JsonBuilder::endSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeSequenceProperty(identity, child_->data(), child_->lengthIdentity_, child_->length_));
	child_.reset();
}

QuickFAST::Messages::ValueMessageBuilder &JsonBuilder::startSequenceEntry(const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new JsonBuilder(mode_, appType, appTypeNs));
	return *child_;
}

void JsonBuilder::endSequenceEntry(QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeSequenceEntryProperty(child_->data()));
	child_.reset();
}

QuickFAST::Messages::ValueMessageBuilder &JsonBuilder::startGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t size) {
	child_.reset(new JsonBuilder(mode_, appType, appTypeNs));
	return *child_;
}

void JsonBuilder::endGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
	add(mode().makeGroupProperty(identity, child_->data()));
	child_.reset();
}

bool JsonBuilder::wantLog(unsigned short level) {
	return false;
}

bool JsonBuilder::logMessage(unsigned short level, const std::string &reason) {
	return true;
}

bool JsonBuilder::reportDecodingError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}

bool JsonBuilder::reportCommunicationError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}
		
void JsonBuilder::add(const std::string &property) {
	if(!strm_.str().empty())
		strm_ << ',';

	strm_ << property;
}

ERL_NIF_TERM JsonBuilder::result() const {
	return data_;
}
		
std::string JsonBuilder::data() const {
	return strm_.str();
}
		
const JsonBuilder::Mode &JsonBuilder::mode() const {
	return *mode_;
}

//// class JsonBuilder::StandardMode
std::string JsonBuilder::StandardMode::makeId(const QuickFAST::Messages::FieldIdentityCPtr &identity) const {
	return identity->id();
}
		
std::string JsonBuilder::StandardMode::makeMessage(const std::string &value) const {
	return '{' + value + '}';
}

std::string JsonBuilder::StandardMode::makeProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const {
	return makeId(identity) + ":\"" + value + '"';
}

std::string JsonBuilder::StandardMode::makeSequenceProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const {
	return makeId(lengthIdentity) + ":[" + value + ']';
}

std::string JsonBuilder::StandardMode::makeSequenceEntryProperty(const std::string &value) const {
	return '{' + value + '}';
}

std::string JsonBuilder::StandardMode::makeGroupProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const {
	return makeId(identity) + ":{" + value + '}';
}

//// class JsonBuilder::ExtendedMode
std::string JsonBuilder::ExtendedMode::makeId(const QuickFAST::Messages::FieldIdentityCPtr &identity) const {
	return "\"tag\":" + (identity->id().empty() ? "null" : ('"' + identity->id() + '"')) + ",\"name\":" + (identity->name().empty() ? "null" : ('"' + identity->name() + '"'));
}
		
std::string JsonBuilder::ExtendedMode::makeMessage(const std::string &value) const {
	return '[' + value + ']';
}

std::string JsonBuilder::ExtendedMode::makeProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const {
	return "{\"type\":\"field\"," + makeId(identity) + ",\"value\":\"" + value + "\"}";
}

std::string JsonBuilder::ExtendedMode::makeSequenceProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const {
	return "{\"type\":\"sequence\"," + makeId(identity) + ",\"value\":[" + value + "],\"length\":{" + makeId(lengthIdentity) + ",\"value\":" + std::to_string(length) + "}}";
}

std::string JsonBuilder::ExtendedMode::makeSequenceEntryProperty(const std::string &value) const {
	return '[' + value + ']';
}

std::string JsonBuilder::ExtendedMode::makeGroupProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const {
	return "{\"type\":\"group\"," + makeId(identity) + ",\"value\":[" + value + "]}";
}

} } // namespace Fastprot::Detail


