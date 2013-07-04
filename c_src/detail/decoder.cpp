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

#include "decoder.h"
#include "termbuilder.h"

namespace Fastprot { namespace Detail {

Decoder::Decoder(std::istream &templateData) {
	std::unique_ptr<Builder> builder(new TermBuilder);
	init(templateData, builder);
}

Decoder::Decoder(std::istream &templateData, std::unique_ptr<Builder> &builder) {
	init(templateData, builder);
}

void Decoder::init(std::istream &templateData, std::unique_ptr<Builder> &builder) {
	builder_ = std::move(builder);
	registry_ = QuickFAST::Codecs::XMLTemplateParser().parse(templateData);
	packetHeaderAnalyzer_.reset(new QuickFAST::Codecs::NoHeaderAnalyzer);
	messageHeaderAnalyzer_.reset(new QuickFAST::Codecs::NoHeaderAnalyzer);
	assembler_.reset(new QuickFAST::Codecs::MessagePerPacketAssembler(
		registry_,
		*messageHeaderAnalyzer_,
		*packetHeaderAnalyzer_,
		*builder_
	));
	
	receiver_.reset(new QuickFAST::Communication::BufferReceiver);
	receiver_->start(*assembler_, 1400, 1);
}

ERL_NIF_TERM Decoder::decode(ErlNifEnv *env, unsigned const char *buffer, size_t size) {
	builder_->setEnv(env);
	receiver_->receiveBuffer(buffer, size);
	ERL_NIF_TERM res = builder_->result();
	builder_->setEnv(nullptr);

	return res;
}
		
ERL_NIF_TERM Decoder::decode(ErlNifEnv *env, const std::string &buffer) {
	return decode(env, reinterpret_cast<unsigned const char *>(buffer.c_str()), buffer.length());
}

} } // namespace Fastprot

