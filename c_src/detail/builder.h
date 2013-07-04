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

#ifndef _FASTPROT_DETAIL_BUILER_H
#define _FASTPROT_DETAIL_BUILER_H

#include "types.h"
#include "../enif_helper.h"

namespace Fastprot { namespace Detail {

class Builder: public QuickFAST::Messages::ValueMessageBuilder {
	public:
		Builder();
		Builder(ErlNifEnv *env, const std::string &appType, const std::string &appTypeNs);
		Builder(const Builder &) = default;
		Builder(Builder &&) = default;
		virtual ~Builder() = default;

		virtual ERL_NIF_TERM result() const = 0;
		void setEnv(ErlNifEnv *env);

		//// ValueMessageBuilder getters
		const std::string &getApplicationType() const;
		const std::string &getApplicationTypeNs() const;

	protected:
		ErlNifEnv *env() const;
		ERL_NIF_TERM makeDone(ERL_NIF_TERM data) const;
		ERL_NIF_TERM makeError(const std::string &reason) const;

		void setAppType(const std::string &appType);
		void setAppTypeNs(const std::string &appTypeNs);

	private:
		ErlNifEnv *env_;
		std::string appType_;
		std::string appTypeNs_;
};

} } // namespace Fastprot::Detail

#endif
