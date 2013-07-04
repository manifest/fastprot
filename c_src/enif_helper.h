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

#ifndef _YANOT_ENIF_HELPER_H
#define _YANOT_ENIF_HELPER_H

#include <erl_nif.h>
#include <cstring>
#include <string>

namespace enif {

template <typename T>
inline void gc_type(ErlNifEnv *env, void *cover) {
	T *obj(static_cast<T *>(*static_cast<void **>(cover)));
	if(obj) {
		delete obj;
		obj = 0;
	}
}

template <typename Object>
inline ErlNifResourceType *create_type(ErlNifEnv *env, const char *name) {
	return enif_open_resource_type(env, NULL, name, gc_type<Object>, ERL_NIF_RT_CREATE, NULL);
}

template <typename Object>
inline void **cover_object(ErlNifResourceType *type, Object *obj) {
	auto ptr(enif_alloc_resource(type, sizeof(Object *)));
	if(!ptr)
		return nullptr;

	void **cover = static_cast<void **>(ptr);
	*cover = obj;
	return cover;
}

template <typename Object> 
inline Object *uncover_object(void *cover) {
	return *static_cast<Object **>(cover);
}

inline ERL_NIF_TERM make_binary(ErlNifEnv *env, const unsigned char *buf, size_t size) {
	ERL_NIF_TERM bin;
	unsigned char *data(enif_make_new_binary(env, size, &bin));
	memcpy(data, buf, size);
	return bin;
}

inline ERL_NIF_TERM make_binary_string(ErlNifEnv *env, const std::string &text) {
	return make_binary(env, reinterpret_cast<const unsigned char *>(text.c_str()), text.size());
}

inline ERL_NIF_TERM make_binary_string(ErlNifEnv *env, const char *text) {
	return make_binary(env, reinterpret_cast<const unsigned char *>(text), std::strlen(text));
}

} // namespace enif

#endif

