%% ------------------------------------------------------------------
%% Copyright 2013 by Andrei Nesterov (ae.nesterov@gmail.com)
%%
%% Permission is hereby granted, free of charge, to any person obtaining a copy
%% of this software and associated documentation files (the "Software"), to
%% deal in the Software without restriction, including without limitation the
%% rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
%% sell copies of the Software, and to permit persons to whom the Software is
%% furnished to do so, subject to the following conditions:
%%
%% The above copyright notice and this permission notice shall be included in
%% all copies or substantial portions of the Software.
%%
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
%% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
%% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
%% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
%% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
%% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
%% IN THE SOFTWARE.
%% ------------------------------------------------------------------

-module(fastprot_srv).
-behaviour(gen_server).
-author('ae.nesterov@gmail.com').

%% API
-export([
	start_link/0,
	decoder/2,
	decode/2
]).

%% Generic Server callbacks
-export([
	init/1,
	handle_call/3,
	handle_cast/2,
	handle_info/2,
	terminate/2,
	code_change/3
]).

-record(state, {}).

%% ===================================================================
%% API functions
%% ===================================================================

start_link() ->
	gen_server:start_link({local, ?MODULE}, ?MODULE, [], []).

decoder(Template, Opts) ->
	gen_server:call(?MODULE, {decoder, Template, Opts}).

decode(Decoder, Message) ->
	gen_server:call(?MODULE, {decode, Decoder, Message}).

%% ===================================================================
%% Generic Server callbacks
%% ===================================================================

init([]) ->
	{ok, #state{}}.

handle_call({decoder, Template, Opts}, From, S) ->
	fastprot_impl:decoder({Template, Opts}, From),
	{noreply, S};
handle_call({decode, Decoder, Message}, From, S) ->
	fastprot_impl:decode({Decoder, Message}, From),
	{noreply, S};
handle_call(_Req, _From, S) ->
  {noreply, S}.

handle_cast(stop, S) ->
	{stop, normal, S};
handle_cast(_Req, S) ->
	{noreply, S}.

handle_info({Resp, Client}, S) ->
	gen_server:reply(Client, Resp),
	{noreply, S};
handle_info(Req, S) ->
	erlang:display(Req),
	{noreply, S}.

terminate(normal, _S) ->
	ok.

code_change(_OldVsn, S, _Extra) ->
	{ok, S}.

