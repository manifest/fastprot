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

#ifndef _FASTPROT_DETAIL_JSON_BUILDER_H
#define _FASTPROT_DETAIL_JSON_BUILDER_H

#include "builder.h"
#include <sstream>

namespace Fastprot { namespace Detail {

class JsonBuilder: public Builder {
	public:
		class Mode;
		class StandardMode;
		class ExtendedMode;

		JsonBuilder();
		explicit JsonBuilder(JsonBuilder::Mode *mode);
		JsonBuilder(const JsonBuilder &) = default;
		JsonBuilder(JsonBuilder &&) = default;

		//// Value
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int64 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint64 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int32 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint32 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int16 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uint16 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::int8 value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::uchar value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const QuickFAST::Decimal& value);
		void addValue(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::ValueType::Type type, const unsigned char *value, size_t length);
		
		//// Message
		QuickFAST::Messages::ValueMessageBuilder &startMessage(const std::string &appType, const std::string &appTypeNs, size_t size);
		bool endMessage(QuickFAST::Messages::ValueMessageBuilder &builder);
		bool ignoreMessage(QuickFAST::Messages::ValueMessageBuilder &builder);

		//// Sequence
		QuickFAST::Messages::ValueMessageBuilder &startSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t fieldCount, QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length);
		void endSequence(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder);
		
		//// SequenceEntry
		QuickFAST::Messages::ValueMessageBuilder &startSequenceEntry(const std::string &appType, const std::string &appTypeNs, size_t size);
		void endSequenceEntry(QuickFAST::Messages::ValueMessageBuilder &builder);
		
		//// Group
		QuickFAST::Messages::ValueMessageBuilder &startGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &appType, const std::string &appTypeNs, size_t size);
		void endGroup(QuickFAST::Messages::FieldIdentityCPtr &identity, QuickFAST::Messages::ValueMessageBuilder &builder);

		//// Logger implementation
		bool wantLog(unsigned short level);
		bool logMessage(unsigned short level, const std::string &logMessage);
		bool reportDecodingError(const std::string &errorMessage);
		bool reportCommunicationError(const std::string &errorMessage);

		const JsonBuilder::Mode &mode() const;
		ERL_NIF_TERM result() const;

	private:
		JsonBuilder(const std::shared_ptr<JsonBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs);
		JsonBuilder(const std::shared_ptr<JsonBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length);

		void add(const std::string &property);
		std::string data() const;

		std::shared_ptr<JsonBuilder::Mode> mode_;
		std::unique_ptr<JsonBuilder> child_;
		std::stringstream strm_;
		ERL_NIF_TERM data_;
		
		QuickFAST::Messages::FieldIdentityCPtr lengthIdentity_;
		size_t length_;
};

class JsonBuilder::Mode {
	public:
		virtual ~Mode() = default;
		virtual std::string makeId(const QuickFAST::Messages::FieldIdentityCPtr &identity) const = 0;
		virtual std::string makeMessage(const std::string &value) const = 0;
		virtual std::string makeProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const = 0;
		virtual std::string makeSequenceProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const = 0;
		virtual std::string makeSequenceEntryProperty(const std::string &value) const = 0;
		virtual std::string makeGroupProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const = 0;
};

class JsonBuilder::StandardMode: public JsonBuilder::Mode {
	public:
		std::string makeId(const QuickFAST::Messages::FieldIdentityCPtr &identity) const;
		std::string makeMessage(const std::string &value) const;
		std::string makeProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const;
		std::string makeSequenceProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const;
		std::string makeSequenceEntryProperty(const std::string &value) const;
		std::string makeGroupProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const;
};

class JsonBuilder::ExtendedMode: public JsonBuilder::Mode {
	public:
		std::string makeId(const QuickFAST::Messages::FieldIdentityCPtr &identity) const;
		std::string makeMessage(const std::string &value) const;
		std::string makeProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const;
		std::string makeSequenceProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const;
		std::string makeSequenceEntryProperty(const std::string &value) const;
		std::string makeGroupProperty(const QuickFAST::Messages::FieldIdentityCPtr &identity, const std::string &value) const;
};

} } // namespace Fastprot::Detail

#endif


