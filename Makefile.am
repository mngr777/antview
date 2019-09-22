AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SOURCE_FILES = \
	ant.hpp \
	ant.cpp \
	data.hpp \
	data.cpp \
	trail_parser.hpp \
	trail_parser.cpp \
	app/sdl.hpp \
	app/sdl.cpp \
	app/texture_manager.hpp \
	app/texture_manager.cpp \
	app/trailedit.hpp \
	app/trailedit.cpp

# TODO: binaries

TESTS = test_trail_parser1

check_PROGRAMS = $(TESTS)

test_trail_parser1_SOURCES = tests/trail_parser1.cpp \
	ant.hpp ant.cpp \
	trail_parser.hpp trail_parser.cpp
