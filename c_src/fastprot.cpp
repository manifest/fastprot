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

#include "enif_helper.h"
#include "enif_types_info.h"
#include "middleware.h"
#include <sstream>
#include <chrono>

using namespace cppa;

namespace {

// ===================================================================
// Global
// ===================================================================

actor_ptr mw_;
ErlNifEnv *env_(nullptr);

// ===================================================================
// API functions
// ===================================================================

ERL_NIF_TERM decoder(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	// Sender's pid
	ErlNifPid pid;
	enif_self(env, &pid);

	// {Params, _} = Args
	int params_count(0);
	const ERL_NIF_TERM *params(nullptr);
	if(!enif_get_tuple(env, argv[0], &params_count, &params))
		return enif_make_badarg(env);

	if(params_count < 2)
		return enif_make_badarg(env);

	// {_, Client} = Args
	auto client(enif_make_copy(env_, argv[1]));

	// {{Template, _}, _} = Args
	ErlNifBinary template_bin;
	if(!enif_inspect_binary(env, params[0], &template_bin))
		return enif_make_badarg(env);
	std::string templ(reinterpret_cast<char *>(template_bin.data), template_bin.size);

	// {{_, Type}, _} = Args
	std::string type;
	ERL_NIF_TERM head, tail = params[1];
	while(enif_get_list_cell(env, tail, &head, &tail)) {
		unsigned int length(0);
		if(!enif_get_atom_length(env, head, &length, ERL_NIF_LATIN1))
			return enif_make_badarg(env);

		std::unique_ptr<char[]> opt(new char[length + 1]);
		if(!enif_get_atom(env, head, &opt[0], length + 1, ERL_NIF_LATIN1))
			return enif_make_badarg(env);
		type = std::string(&opt[0], length);
	}

	// Delegating control to the actor
	send(mw_, atom("decoder"), pid, templ, type, client);

	// ok
	return enif_make_atom(env, "ok");
}

ERL_NIF_TERM decode(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	// Sender's pid
	ErlNifPid pid;
	enif_self(env, &pid);

	// {Params, _} = Args
	int params_count(0);
	const ERL_NIF_TERM *params(nullptr);
	if(!enif_get_tuple(env, argv[0], &params_count, &params))
		return enif_make_badarg(env);

	if(params_count < 2)
		return enif_make_badarg(env);

	// {_, Client} = Args
	auto client(enif_make_copy(env_, argv[1]));

	// {{Decoder, _}, _} = Args
	Fastprot::Middleware::DecoderId id(0);
	if(!enif_get_uint(env, params[0], &id))
		return enif_make_badarg(env);

	// {{_, Message}, _} = Args
	ErlNifBinary message_bin;
	if(!enif_inspect_binary(env, params[1], &message_bin))
		return enif_make_badarg(env);
	std::string message(reinterpret_cast<char *>(message_bin.data), message_bin.size);

	// Delegating control to the actor
	send(mw_, atom("decode"), pid, id, message, client);

	// ok
	return enif_make_atom(env, "ok");
}

ErlNifFunc funcs[] = {
	{"decoder", 2, decoder},
	{"decode", 2, decode}
};

// ===================================================================
// Module callbacks
// ===================================================================

int load(ErlNifEnv * /*env*/, void ** /*data*/, ERL_NIF_TERM /*info*/) {
	env_ = enif_alloc_env();

	ErlNifPid_TypeInfo::announce();
	mw_ = spawn<Fastprot::Middleware>();
	return 0;
}

void unload(ErlNifEnv * /*env*/, void * /*priv_data*/) {
	send(mw_, atom("kill"));
  await_all_others_done();
	shutdown();

	enif_free_env(env_);
}

} // namespace <anonymous>

ERL_NIF_INIT(fastprot_impl, funcs, load, NULL, NULL, unload)

