# Fastprot

FAST protocol message decoder for Erlang.

### Depends on

- [boost][boost] (boost_system, boost_thread) - free peer-reviewed portable C++ source libraries
- [cppa][cppa] - implementation of the actor model for C++
- [xerces-c][xerces-c] - XML parser
- [QuickFAST][quickfast] - implementation of the FAST protocol for native C++

### Getting Started

1. Build and install dependencies.

	> ___Important!__ On OSX you must link libc++ library instead of libstdc++ when building dependency libraries,  in case of using Clang compiler._

2. Build application:

		$ make

3. It is recommended to run the unit tests as well:

		$ make tests

### Hello, World!

1. Run application:

		$ make start

2. Read example of the template file and example of encoded message:

		1> {ok, Template} = file:read_file("../../test/data/templates/rtsx.xml").
		{ok,<<"<templates xmlns=\"http://www.fixprotocol.org/ns/fast/td/1.1\">\n\n...>>}

		2> {ok, EncMsg} = file:read_file("../../test/data/fast-message-example").
		{ok,<<192,248,224,202,11,13,247,35,97,7,65,113,4,114,152,
					131,216,129,176,21,68,108,199,32,247,5,116,...>>}

3. Create decoder:

		3> {ok, Dec} = fastprot:decoder(Template).
		{ok,0}

4. Use decoder for decoding message:

		4> fastprot:decode(Dec, EncMsg).               
		{ok,[{1128,<<"8">>},
				{35,<<"X">>},
				{49,<<"RTS">>},
				{34,182007},
				{52,20130117084854552},
				{268,
					[[{279,1},
						{269,<<"0">>},
						{48,45168198},
						{22,8},
						{5842,4214},
						{83,96877},
						{264,5},
						{1023,1},
						{270,30560.0},
						{271,5},
						{273,84927499}],
					[{279,2},
						{269,<<"0">>},
						{48,49664582},
						{22,8},
						{5842,4214},
						{83,6311},
						{264,5},
						{1023,3},
						{270,31345.0},
						{271,0},
						{273,84927507}],
					[{279,2},
						{269,<<"0">>},
						{48,49666118},
						{22,8},
						{5842,4214},
						{83,2975},
						{264,5},
						{1023,2},
						{270,32055.0},
						{271,0},
						{273,84927513}]]}]}

### Async World

It is possible decoding messages asynchronously. Use `fastprot:async` module for that.

1. Create decoder:

		5> fastprot_async:decoder(Template).
		#Ref<0.0.0.66>

		6> flush().
		Shell got {{ok,0},#Ref<0.0.0.66>}
		ok

2. Use decoder for decoding message:

		7> fastprot_async:decode(0, EncMsg).
		#Ref<0.0.0.74>

		8> flush().
		Shell got {{ok,[{1128,<<"8">>},
										{35,<<"X">>},
										{49,<<"RTS">>},
										{34,182007},
										{52,20130117084854552},
										{268,
											[[{279,1},
												{269,<<"0">>},
												{48,45168198},
												{22,8},
												{5842,4214},
												{83,96877},
												{264,5},
												{1023,1},
												{270,30560.0},
												{271,5},
												{273,84927499}],
											[{279,2},
												{269,<<"0">>},
												{48,49664582},
												{22,8},
												{5842,4214},
												{83,6311},
												{264,5},
												{1023,3},
												{270,31345.0},
												{271,0},
												{273,84927507}],
											[{279,2},
												{269,<<"0">>},
												{48,49666118},
												{22,8},
												{5842,4214},
												{83,2975},
												{264,5},
												{1023,2},
												{270,32055.0},
												{271,0},
												{273,84927513}]]}]},
							#Ref<0.0.0.74>}
		ok

### Available decoder's modes

It is possible to specify decoding output format. Those are supported:

- __term__
List of items representing a pair of tag (integer) and a value (binary string) or a list of such pairs.
- __term_ex__
Contains more information, items in the list is represented by records (value, sequence, group) and may include such properties as: tag, name, value (all are binary strings).
- __fix__
FIX protocol message.
- __json__
Same as __term__ mode, but represented as a binary string and in the json format.
- __json_ex__
Contains more information, items may include such properties as: type, tag, name, value.

Sync way:

	fastprot:decoder(Template, [json]).

And async:

	fastprot_async:decoder(Template, [json]).

### License

Fastprot is provided under the terms of [the MIT license][license]

[boost]:http://www.boost.org
[cppa]:https://github.com/Neverlord/libcppa
[xerces-c]:http://xerces.apache.org/xerces-c
[quickfast]:http://code.google.com/p/quickfast
[license]:http://www.opensource.org/licenses/MIT

