all: release

deps:
	./rebar get-deps

deps-update:
	./rebar update-deps

build:
	./rebar compile

release: build
	./rebar generate

clean:
	./rebar clean

distclean: clean
	./rebar delete-deps

start:
	cd rel/fastprot && sh bin/fastprot console

tests: build
	./rebar -v skip_deps=true eunit

