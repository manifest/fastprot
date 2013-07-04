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

#include "middleware.h"
#include "exit_reason.h"
#include "detail/decoder.h"
#include "detail/termbuilder.h"
#include "detail/fixbuilder.h"
#include "detail/jsonbuilder.h"
#include <stdexcept>
#include <sstream>

using namespace cppa;

namespace {

std::unique_ptr<Fastprot::Detail::Builder> createBuilder(const std::string &type) {
	using namespace Fastprot::Detail;
	
	std::unique_ptr<Builder> builder;
	if(type.compare("term") == 0)
		builder.reset(new TermBuilder(new TermBuilder::StandardMode));
	else if(type.compare("term_ex") == 0)
		builder.reset(new TermBuilder(new TermBuilder::ExtendedMode));
	else if(type.compare("fix") == 0)
		builder.reset(new FixBuilder);
	else if(type.compare("json") == 0)
		builder.reset(new JsonBuilder(new JsonBuilder::StandardMode));
	else if(type.compare("json_ex") == 0)
		builder.reset(new JsonBuilder(new JsonBuilder::ExtendedMode));

	return builder;
}

void sendSuccess(const ErlNifPid *pid, ErlNifEnv *env, ERL_NIF_TERM response, ERL_NIF_TERM client) {
	// {Resourse, Client}
	enif_send(nullptr, pid, env, 
		enif_make_tuple2(env,
			response,
			client
		)
	);
}

void sendError(const ErlNifPid *pid, ErlNifEnv *env, const std::string &reason, ERL_NIF_TERM client) {
	// {{error, Reason}, Client}
	enif_send(nullptr, pid, env, 
		enif_make_tuple2(env,
			enif_make_tuple2(env,
				enif_make_atom(env, "error"),
				enif::make_binary_string(env, reason)
			),
			client
		)
	);
}

} // namespace <ananymous>

namespace Fastprot {

void Middleware::init() {
	become(
		on(atom("decode"), arg_match) >> [this](const ErlNifPid &pid, DecoderId id, const std::string &message, ERL_NIF_TERM client) {
			reply(atom("ok"));
			auto env(enif_alloc_env());

			Decoders::iterator it(decoders_.find(id));
			if(it != decoders_.end()) {
				auto res(it->second->decode(env, message));
				sendSuccess(&pid, env, res, enif_make_copy(env, client));
			}
			else
				sendError(&pid, env, "Wrong decoder.", enif_make_copy(env, client));
		},
		on(atom("decoder"), arg_match) >> [this](const ErlNifPid &pid, const std::string &templateData, const std::string &type, ERL_NIF_TERM client) {
			reply(atom("ok"));
			auto env(enif_alloc_env());

			auto builder(createBuilder(type));
			if(builder) {
				try {
					std::istringstream sstrm(templateData);
					std::istream strm(sstrm.rdbuf());

					auto decoder(std::shared_ptr<Detail::Decoder>(new Detail::Decoder(strm, builder)));
					DecoderId id(decoders_.size());
					decoders_.insert(std::make_pair(id, decoder));
					auto res(enif_make_tuple2(env,
						enif_make_atom(env, "ok"),
						enif_make_uint(env, id))
					);

					sendSuccess(&pid, env, res, enif_make_copy(env, client));
				}
				catch(std::exception &e) {
					sendError(&pid, env, e.what(), enif_make_copy(env, client));
				}
				catch(...) {
					sendError(&pid, env, "Internal error.", enif_make_copy(env, client));
				}
			}
			else
				sendError(&pid, env, "Wrong type.", enif_make_copy(env, client));
		},
		on(atom("kill")) >> [this]() {
			reply(atom("ok"));
			quit(ExitReason::Killed);
		}
	);
}

} // namespace Fastprot


