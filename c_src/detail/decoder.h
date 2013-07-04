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

#ifndef _FASTPROT_DETAIL_DECODER_H
#define _FASTPROT_DETAIL_DECODER_H

#include "types.h"
#include "builder.h"

namespace Fastprot { namespace Detail {

class Decoder {
	public:
		Decoder(std::istream &templateData);
		Decoder(std::istream &templateData, std::unique_ptr<Builder> &builder);
		
		ERL_NIF_TERM decode(ErlNifEnv *env, unsigned const char *buffer, size_t size);
		ERL_NIF_TERM decode(ErlNifEnv *env, const std::string &buffer);

	private:
		void init(std::istream &templateData, std::unique_ptr<Builder> &builder);

		std::unique_ptr<Builder> builder_;
		std::unique_ptr<QuickFAST::Codecs::HeaderAnalyzer> packetHeaderAnalyzer_;
		std::unique_ptr<QuickFAST::Codecs::HeaderAnalyzer> messageHeaderAnalyzer_;
		std::unique_ptr<QuickFAST::Communication::Assembler> assembler_;
		std::unique_ptr<QuickFAST::Communication::Receiver> receiver_;
		QuickFAST::Codecs::TemplateRegistryPtr registry_;
};

} } // namespace Fastprot::Detail

#endif

