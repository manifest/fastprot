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

#ifndef _FASTPROT_DETAIL_TERM_BUILDER_H
#define _FASTPROT_DETAIL_TERM_BUILDER_H

#include "builder.h"

namespace Fastprot { namespace Detail {

class TermBuilder: public Builder {
	public:
		class Mode;
		class StandardMode;
		class ExtendedMode;

		TermBuilder();
		explicit TermBuilder(TermBuilder::Mode *mode);
		TermBuilder(const TermBuilder &) = default;
		TermBuilder(TermBuilder &&) = default;

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

		const TermBuilder::Mode &mode() const;
		ERL_NIF_TERM result() const;

	private:
		TermBuilder(ErlNifEnv *env, const std::shared_ptr<TermBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs);
		TermBuilder(ErlNifEnv *env, const std::shared_ptr<TermBuilder::Mode> &mode, const std::string &appType, const std::string &appTypeNs, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length);

		void add(ERL_NIF_TERM property);

		std::shared_ptr<TermBuilder::Mode> mode_;
		std::unique_ptr<TermBuilder> child_;
		ERL_NIF_TERM data_;
		
		QuickFAST::Messages::FieldIdentityCPtr lengthIdentity_;
		size_t length_;
};

class TermBuilder::Mode {
	public:
		virtual ~Mode() = default;
		virtual ERL_NIF_TERM makeId(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity) const = 0;
		virtual ERL_NIF_TERM makeMessage(ErlNifEnv *env, ERL_NIF_TERM value) const = 0;
		virtual ERL_NIF_TERM makeProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const = 0;
		virtual ERL_NIF_TERM makeSequenceProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const = 0;
		virtual ERL_NIF_TERM makeSequenceEntryProperty(ErlNifEnv *env, ERL_NIF_TERM value) const = 0;
		virtual ERL_NIF_TERM makeGroupProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const = 0;
};

class TermBuilder::StandardMode: public TermBuilder::Mode {
	public:
		ERL_NIF_TERM makeId(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity) const;
		ERL_NIF_TERM makeMessage(ErlNifEnv *env, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeSequenceProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const;
		ERL_NIF_TERM makeSequenceEntryProperty(ErlNifEnv *env, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeGroupProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const;
};

class TermBuilder::ExtendedMode: public TermBuilder::Mode {
	public:
		ERL_NIF_TERM makeId(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity) const;
		ERL_NIF_TERM makeMessage(ErlNifEnv *env, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeSequenceProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value, const QuickFAST::Messages::FieldIdentityCPtr &lengthIdentity, size_t length) const;
		ERL_NIF_TERM makeSequenceEntryProperty(ErlNifEnv *env, ERL_NIF_TERM value) const;
		ERL_NIF_TERM makeGroupProperty(ErlNifEnv *env, const QuickFAST::Messages::FieldIdentityCPtr &identity, ERL_NIF_TERM value) const;
};

} } // namespace Fastprot::Detail

#endif

