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

#include "fixbuilder.h"
#include <cassert>

namespace {

template <typename T>
inline void addProperty(std::stringstream &strm, const QuickFAST::Messages::FieldIdentityCPtr &identity, const T &value) {
	strm << identity->id() << '=' << value << '\x01';
}

} // namespace <ananymous>

namespace Fastprot { namespace Detail {
		
void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int64 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint64 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int32 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint32 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int16 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint16 value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int8 value) {
	addProperty(strm_, identity, static_cast<std::int16_t>(value));
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uchar value) {
	addProperty(strm_, identity, static_cast<std::uint16_t>(value));
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::Decimal& value) {
	addProperty(strm_, identity, value);
}

void FixBuilder::addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const unsigned char *value, size_t length) {
	addProperty(strm_, identity, std::string(reinterpret_cast<const char *>(value), length));
}

QuickFAST::Messages::ValueMessageBuilder &FixBuilder::startMessage(const std::string &appType, const std::string &appTypeNs, size_t size) {
	data_ = ERL_NIF_TERM();
	setAppType(appType);
	setAppTypeNs(appTypeNs);
	return *this;
}

bool FixBuilder::endMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	data_ = makeDone(enif::make_binary_string(env(), strm_.str()));
	strm_.str(std::string());
	strm_.clear();
	return true;
}

bool FixBuilder::ignoreMessage(QuickFAST::Messages::ValueMessageBuilder &builder) {
	return true;
}

QuickFAST::Messages::ValueMessageBuilder &FixBuilder::startSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t fieldCount, QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) {
	addProperty(strm_, lengthIdentity, length);
	return *this;
}

void FixBuilder::endSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
}

QuickFAST::Messages::ValueMessageBuilder &FixBuilder::startSequenceEntry(const std::string &appType, const std::string &appTypeNs, size_t size) {
	return *this;
}

void FixBuilder::endSequenceEntry(QuickFAST::Messages::ValueMessageBuilder &builder) {
}

QuickFAST::Messages::ValueMessageBuilder &FixBuilder::startGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t size) {
	addProperty(strm_, identity, size);
	return *this;
}

void FixBuilder::endGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder) {
}

bool FixBuilder::wantLog(unsigned short level) {
	return false;
}

bool FixBuilder::logMessage(unsigned short level, const std::string &reason) {
	return true;
}

bool FixBuilder::reportDecodingError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}

bool FixBuilder::reportCommunicationError(const std::string &reason) {
	data_ = makeError(reason);
	return false;
}

ERL_NIF_TERM FixBuilder::result() const {
	return data_;
}

} } // namespace Fastprot::Detail


