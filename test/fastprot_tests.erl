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

-module(fastprot_tests).
-author('ae.nesterov@gmail.com').

-include_lib("eunit/include/eunit.hrl").

%% ==================================================================
%% Decoder
%% ==================================================================

decoder(Config) ->
	{inparallel, [
		{"Right type",
			[{lists:flatten(io_lib:format("~s mode", [M])), decoder_right_type(M, Config)} || M <- decoder_modes()]},
		{"Wrong type",
			decoder_wrong_type(Config)}
	]}.

decoder_right_type(Mode, Config) ->
	TemplateBin = proplists:get_value(template, Config),
	{Status, Dec} = fastprot:decoder(TemplateBin, [Mode]),
	[
		{"The successful creation of a decoder",
			?_assertEqual(ok, Status)},
		{"Decoder identifier is represented as integer",
			?_assert(erlang:is_integer(Dec))}
	].

decoder_wrong_type(Config) ->
	TemplateBin = proplists:get_value(template, Config),
	[
		{"Type is unsupported", 
			?_assertEqual({error, <<"Wrong type.">>}, fastprot:decoder(TemplateBin, [qwerty]))},
		{"Type doesn't set", 
			?_assertEqual({error, <<"Wrong type.">>}, fastprot:decoder(TemplateBin, []))}
	].

%% ==================================================================
%% Decode
%% ==================================================================

decode_right_decoder(Mode, Dec) ->
	[
		{"Right encoded message",
			?_assertEqual({ok, decoded_message(Mode)}, fastprot:decode(Dec, encoded_message()))},
		{"Wrong encoded message",
			?_assertEqual({error, <<"[ERR U03] EOF while decoding presence map.">>}, fastprot:decode(Dec, <<0>>))}
	].

decode_wrong_decoder(Dec) ->
	{"Positive id",
		?_assertEqual({error, <<"Wrong decoder.">>}, fastprot:decode(Dec, <<0>>))}.

decode_message(Config) ->
	Decs = proplists:get_value(decoders, Config),
	{_, LastDec}  = lists:last(Decs),
	[
		{"Right decoder",
			[{lists:flatten(io_lib:format("~s mode", [M])), decode_right_decoder(M, D)} || {M, D} <- Decs]},
		{"Wrong decoder",
			decode_wrong_decoder(LastDec + 1)}
	].

decode(Config) ->
	{inparallel,
		{setup,
			fun()        -> decode_group_begin(Config) end,
			fun(Config2) -> decode_group_end(Config2) end,
			fun(Config2) -> decode_message(Config2) end
		}
	}.

decode_group_begin(Config) ->
	TemplateBin = proplists:get_value(template, Config),
	Decs = [{M, D} || {M, {ok, D}} <- [{M, fastprot:decoder(TemplateBin, [M])} || M <- decoder_modes()]],
	[{decoders, Decs} | Config].

decode_group_end(_Config) ->
	ok.

%% ==================================================================
%% Test suite setup
%% ==================================================================

suite_main(Config) ->
	{"FAST Protocol Analyzer", {inorder, [
		{"Creating decoder", decoder(Config)},
		{"Decoding message", decode(Config)}
	]}}.

suite_begin() ->
	ok = application:start(fastprot),
	{ok, TemplateBin} = th:read_data_file("templates/rtsx.xml"),
	[{template, TemplateBin}].

suite_end(_Config) ->
	application:stop(fastprot),
	ok.

%% ==================================================================
%% Test suite run function
%% ==================================================================

fastprot_test_() ->
	{setup,
		fun suite_begin/0,
		fun suite_end/1,
		fun suite_main/1
	}.

%% ==================================================================
%% Stress Test suite
%% ==================================================================
%
%stress(Mode, Dec) ->
%	{inparallel,
%		[?_assertEqual({ok, decoded_message(Mode)}, fastprot:decode(Dec, encoded_message())) || N <- lists:seq(1, 1000)]
%	}.
%
%stress_suite_test_() ->
%	{setup,
%		fun()       -> decode_group_begin(suite_begin()) end,
%		fun(Config) -> suite_end(decode_group_end(Config)) end,
%		fun(Config) ->
%			[stress(M, D) || {M, D} <- proplists:get_value(decoders, Config)]
%		end
%	}.

%% ==================================================================
%% Helpers
%% ==================================================================

decoder_modes() ->
	[term, term_ex, fix, json, json_ex].

decoded_message(term) ->
	[
		{1128,<<"8">>},
		{35,<<"X">>},
		{49,<<"RTS">>},
		{34,182007},
		{52,20130117084854552},
		{268, [[
			{279,1},
			{269,<<"0">>},
			{48,45168198},
			{22,8},
			{5842,4214},
			{83,96877},
			{264,5},
			{1023,1},
			{270,30560.0},
			{271,5},
			{273,84927499}
		], [
			{279,2},
			{269,<<"0">>},
			{48,49664582},
			{22,8},
			{5842,4214},
			{83,6311},
			{264,5},
			{1023,3},
			{270,31345.0},
			{271,0},
			{273,84927507}
		], [
			{279,2},
			{269,<<"0">>},
			{48,49666118},
			{22,8},
			{5842,4214},
			{83,2975},
			{264,5},
			{1023,2},
			{270,32055.0},
			{271,0},
			{273,84927513}
		]]}
	];
decoded_message(term_ex) ->
	[
		{field,{identity,<<"1128">>,<<"ApplVerID">>},<<"8">>},
		{field,{identity,<<"35">>,<<"MessageType">>},<<"X">>},
		{field,{identity,<<"49">>,<<"SenderCompID">>},<<"RTS">>},
		{field,{identity,<<"34">>,<<"MsgSeqNum">>},182007},
		{field,{identity,<<"52">>,<<"SendingTime">>},20130117084854552},
		{sequence,{identity,<<>>,<<"MDEntries">>}, [
			[
				{field,{identity,<<"279">>,<<"MDUpdateAction">>},1},
				{field,{identity,<<"269">>,<<"MDEntryType">>},<<"0">>},
				{field,{identity,<<"48">>,<<"SecurityID">>},45168198},
				{field,{identity,<<"22">>,<<"SecurityIDSource">>},8},
				{field,{identity,<<"5842">>,<<"ExchangeTradingSessionID">>},4214},
				{field,{identity,<<"83">>,<<"RptSeq">>},96877},
				{field,{identity,<<"264">>,<<"MarketDepth">>},5},
				{field,{identity,<<"1023">>,<<"MDPriceLevel">>},1},
				{field,{identity,<<"270">>,<<"MDEntryPx">>},30560.0},
				{field,{identity,<<"271">>,<<"MDEntrySize">>},5},
				{field,{identity,<<"273">>,<<"MDEntryTime">>},84927499}
			], [
				{field,{identity,<<"279">>,<<"MDUpdateAction">>},2},
				{field,{identity,<<"269">>,<<"MDEntryType">>},<<"0">>},
				{field,{identity,<<"48">>,<<"SecurityID">>},49664582},
				{field,{identity,<<"22">>,<<"SecurityIDSource">>},8},
				{field,{identity,<<"5842">>,<<"ExchangeTradingSessionID">>},4214},
				{field,{identity,<<"83">>,<<"RptSeq">>},6311},
				{field,{identity,<<"264">>,<<"MarketDepth">>},5},
				{field,{identity,<<"1023">>,<<"MDPriceLevel">>},3},
				{field,{identity,<<"270">>,<<"MDEntryPx">>},31345.0},
				{field,{identity,<<"271">>,<<"MDEntrySize">>},0},
				{field,{identity,<<"273">>,<<"MDEntryTime">>},84927507}
			], [
				{field,{identity,<<"279">>,<<"MDUpdateAction">>},2},
				{field,{identity,<<"269">>,<<"MDEntryType">>},<<"0">>},
				{field,{identity,<<"48">>,<<"SecurityID">>},49666118},
				{field,{identity,<<"22">>,<<"SecurityIDSource">>},8},
				{field,{identity,<<"5842">>,<<"ExchangeTradingSessionID">>},4214},
				{field,{identity,<<"83">>,<<"RptSeq">>},2975},
				{field,{identity,<<"264">>,<<"MarketDepth">>},5},
				{field,{identity,<<"1023">>,<<"MDPriceLevel">>},2},
				{field,{identity,<<"270">>,<<"MDEntryPx">>},32055.0},
				{field,{identity,<<"271">>,<<"MDEntrySize">>},0},
				{field,{identity,<<"273">>,<<"MDEntryTime">>},84927513}
			]], {length,{identity,<<"268">>,<<"NoMDEntries">>},3}
		}
	];
decoded_message(fix) ->
	<<49,49,50,56,61,56,1,51,53,61,88,1,52,57,61,82,84,83,1,51,52,61,49,56,50,48,
	48,55,1,53,50,61,50,48,49,51,48,49,49,55,48,56,52,56,53,52,53,53,50,1,50,54,
	56,61,51,1,50,55,57,61,49,1,50,54,57,61,48,1,52,56,61,52,53,49,54,56,49,57,
	56,1,50,50,61,56,1,53,56,52,50,61,52,50,49,52,1,56,51,61,57,54,56,55,55,1,50,
	54,52,61,53,1,49,48,50,51,61,49,1,50,55,48,61,51,48,53,54,48,1,50,55,49,61,
	53,1,50,55,51,61,56,52,57,50,55,52,57,57,1,50,55,57,61,50,1,50,54,57,61,48,1,
	52,56,61,52,57,54,54,52,53,56,50,1,50,50,61,56,1,53,56,52,50,61,52,50,49,52,
	1,56,51,61,54,51,49,49,1,50,54,52,61,53,1,49,48,50,51,61,51,1,50,55,48,61,51,
	49,51,52,53,1,50,55,49,61,48,1,50,55,51,61,56,52,57,50,55,53,48,55,1,50,55,
	57,61,50,1,50,54,57,61,48,1,52,56,61,52,57,54,54,54,49,49,56,1,50,50,61,56,1,
	53,56,52,50,61,52,50,49,52,1,56,51,61,50,57,55,53,1,50,54,52,61,53,1,49,48,
	50,51,61,50,1,50,55,48,61,51,50,48,53,53,1,50,55,49,61,48,1,50,55,51,61,56,
	52,57,50,55,53,49,51,1>>;
decoded_message(json) ->
	<<"{1128:\"8\",35:\"X\",49:\"RTS\",34:\"182007\",52:\"20130117084854552\",268:[{279:\"1\",269:\"0\",48:\"45168198\",22:\"8\",5842:\"4214\",83:\"96877\",264:\"5\",1023:\"1\",270:\"30560.000000\",271:\"5\",273:\"84927499\"},{279:\"2\",269:\"0\",48:\"49664582\",22:\"8\",5842:\"4214\",83:\"6311\",264:\"5\",1023:\"3\",270:\"31345.000000\",271:\"0\",273:\"84927507\"},{279:\"2\",269:\"0\",48:\"49666118\",22:\"8\",5842:\"4214\",83:\"2975\",264:\"5\",1023:\"2\",270:\"32055.000000\",271:\"0\",273:\"84927513\"}]}">>;
decoded_message(json_ex) ->
	<<"[{\"type\":\"field\",\"tag\":\"1128\",\"name\":\"ApplVerID\",\"value\":\"8\"},{\"type\":\"field\",\"tag\":\"35\",\"name\":\"MessageType\",\"value\":\"X\"},{\"type\":\"field\",\"tag\":\"49\",\"name\":\"SenderCompID\",\"value\":\"RTS\"},{\"type\":\"field\",\"tag\":\"34\",\"name\":\"MsgSeqNum\",\"value\":\"182007\"},{\"type\":\"field\",\"tag\":\"52\",\"name\":\"SendingTime\",\"value\":\"20130117084854552\"},{\"type\":\"sequence\",\"tag\":null,\"name\":\"MDEntries\",\"value\":[[{\"type\":\"field\",\"tag\":\"279\",\"name\":\"MDUpdateAction\",\"value\":\"1\"},{\"type\":\"field\",\"tag\":\"269\",\"name\":\"MDEntryType\",\"value\":\"0\"},{\"type\":\"field\",\"tag\":\"48\",\"name\":\"SecurityID\",\"value\":\"45168198\"},{\"type\":\"field\",\"tag\":\"22\",\"name\":\"SecurityIDSource\",\"value\":\"8\"},{\"type\":\"field\",\"tag\":\"5842\",\"name\":\"ExchangeTradingSessionID\",\"value\":\"4214\"},{\"type\":\"field\",\"tag\":\"83\",\"name\":\"RptSeq\",\"value\":\"96877\"},{\"type\":\"field\",\"tag\":\"264\",\"name\":\"MarketDepth\",\"value\":\"5\"},{\"type\":\"field\",\"tag\":\"1023\",\"name\":\"MDPriceLevel\",\"value\":\"1\"},{\"type\":\"field\",\"tag\":\"270\",\"name\":\"MDEntryPx\",\"value\":\"30560.000000\"},{\"type\":\"field\",\"tag\":\"271\",\"name\":\"MDEntrySize\",\"value\":\"5\"},{\"type\":\"field\",\"tag\":\"273\",\"name\":\"MDEntryTime\",\"value\":\"84927499\"}],[{\"type\":\"field\",\"tag\":\"279\",\"name\":\"MDUpdateAction\",\"value\":\"2\"},{\"type\":\"field\",\"tag\":\"269\",\"name\":\"MDEntryType\",\"value\":\"0\"},{\"type\":\"field\",\"tag\":\"48\",\"name\":\"SecurityID\",\"value\":\"49664582\"},{\"type\":\"field\",\"tag\":\"22\",\"name\":\"SecurityIDSource\",\"value\":\"8\"},{\"type\":\"field\",\"tag\":\"5842\",\"name\":\"ExchangeTradingSessionID\",\"value\":\"4214\"},{\"type\":\"field\",\"tag\":\"83\",\"name\":\"RptSeq\",\"value\":\"6311\"},{\"type\":\"field\",\"tag\":\"264\",\"name\":\"MarketDepth\",\"value\":\"5\"},{\"type\":\"field\",\"tag\":\"1023\",\"name\":\"MDPriceLevel\",\"value\":\"3\"},{\"type\":\"field\",\"tag\":\"270\",\"name\":\"MDEntryPx\",\"value\":\"31345.000000\"},{\"type\":\"field\",\"tag\":\"271\",\"name\":\"MDEntrySize\",\"value\":\"0\"},{\"type\":\"field\",\"tag\":\"273\",\"name\":\"MDEntryTime\",\"value\":\"84927507\"}],[{\"type\":\"field\",\"tag\":\"279\",\"name\":\"MDUpdateAction\",\"value\":\"2\"},{\"type\":\"field\",\"tag\":\"269\",\"name\":\"MDEntryType\",\"value\":\"0\"},{\"type\":\"field\",\"tag\":\"48\",\"name\":\"SecurityID\",\"value\":\"49666118\"},{\"type\":\"field\",\"tag\":\"22\",\"name\":\"SecurityIDSource\",\"value\":\"8\"},{\"type\":\"field\",\"tag\":\"5842\",\"name\":\"ExchangeTradingSessionID\",\"value\":\"4214\"},{\"type\":\"field\",\"tag\":\"83\",\"name\":\"RptSeq\",\"value\":\"2975\"},{\"type\":\"field\",\"tag\":\"264\",\"name\":\"MarketDepth\",\"value\":\"5\"},{\"type\":\"field\",\"tag\":\"1023\",\"name\":\"MDPriceLevel\",\"value\":\"2\"},{\"type\":\"field\",\"tag\":\"270\",\"name\":\"MDEntryPx\",\"value\":\"32055.000000\"},{\"type\":\"field\",\"tag\":\"271\",\"name\":\"MDEntrySize\",\"value\":\"0\"},{\"type\":\"field\",\"tag\":\"273\",\"name\":\"MDEntryTime\",\"value\":\"84927513\"}]],\"length\":{\"tag\":\"268\",\"name\":\"NoMDEntries\",\"value\":3}}]">>.

encoded_message() ->
	<<192,248,224,202,11,13,247,35,97,7,65,113,4,114,152,131,216,129,176,21,68,108,199,32,247,5,116,237,134,130,128,129,1,110,224,134,128,40,63,72,139,128,128,128,128,128,192,130,176,2,18,56,129,49,167,132,128,129,1,116,241,129,128,136,128,128,128,128,128,128,176,12,129,23,159,131,128,129,1,122,183,129,128,134,128,128,128,128,128>>.

