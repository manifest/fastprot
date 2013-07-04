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

-module(fastprot_impl).
-author('ae.nesterov@gmail.com').

%% API
-export([
	decoder/2,
	decode/2
]).

-on_load(init/0).

%% ===================================================================
%% API functions
%% ===================================================================

decoder({_Template, _Opts}, _Client) ->
	erlang:nif_error(nif_library_not_loaded).

decode({_Decoder, _Message}, _Client) ->
	erlang:nif_error(nif_library_not_loaded).

%% ===================================================================
%% Initialization
%% ===================================================================

init() ->
	case code:which(fastprot_impl) of
		Filename when is_list(Filename) ->
			erlang:load_nif(filename:join([filename:dirname(Filename), "..", "priv", "fastprot"]), []);
		Reason when is_atom(Reason) ->
			{error, Reason}
	end.

