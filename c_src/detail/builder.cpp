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

#include "builder.h"
#include <cassert>

namespace Fastprot { namespace Detail {

Builder::Builder() 
: env_(nullptr) {
}

Builder::Builder(ErlNifEnv *env, const std::string &appType, const std::string &appTypeNs) 
: env_(env), appType_(appType), appTypeNs_(appTypeNs) {
}

ErlNifEnv *Builder::env() const {
	assert(env_);
	return env_;
}

const std::string &Builder::getApplicationType() const {
	return appType_;
}

const std::string &Builder::getApplicationTypeNs() const {
	return appTypeNs_;
}
		
void Builder::setEnv(ErlNifEnv *env) {
	env_ = env;
}

void Builder::setAppType(const std::string &appType) {
	appType_ = appType;
}

void Builder::setAppTypeNs(const std::string &appTypeNs) {
	appTypeNs_ = appTypeNs;
}
		
ERL_NIF_TERM Builder::makeDone(ERL_NIF_TERM data) const {
	return enif_make_tuple2(env(),
		enif_make_atom(env(), "ok"),
		data
	);
}

ERL_NIF_TERM Builder::makeError(const std::string &reason) const {
	return enif_make_tuple2(env(),
		enif_make_atom(env(), "error"),
		enif::make_binary_string(env(), reason)
	);
}

} } // namespace Fastprot::Detail

