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

#ifndef _FASTPROT_DETAIL_TYPES_H
#define _FASTPROT_DETAIL_TYPES_H

//// Note: quickfast headers
#include <Common/QuickFASTPch.h>
#include <Common/Types.h>
#include <Common/Decimal.h>
#include <Codecs/GenericMessageBuilder.h>
#include <Codecs/MessageConsumer.h>
#include <Codecs/NoHeaderAnalyzer.h>
#include <Codecs/StreamingAssembler.h>
#include <Codecs/TemplateRegistry.h>
#include <Codecs/XMLTemplateParser.h>
#include <Codecs/MessagePerPacketAssembler.h>
#include <Messages/Message.h>
#include <Messages/Sequence.h>
#include <Messages/MessageFormatter.h>
#include <Communication/BufferReceiver.h>

std::string to_string(const QuickFAST::Messages::Message &message);

#endif

