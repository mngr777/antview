AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SOURCES_COMMON = \
	ant.hpp \
	ant.cpp \
	data.hpp \
	data.cpp \
	trail_parser.hpp \
	trail_parser.cpp

SOURCES_SDL =  \
	app/sdl.hpp \
	app/sdl.cpp \
	app/texture_manager.hpp \
	app/texture_manager.cpp

dist_pkgdata_DATA = assets/ant.png assets/food.png

LIBS_SDL = -lSDL2 -lSDL2_image


# Programs
bin_PROGRAMS = trail_editor evolve_ant ant_viewer

# Editor
trail_editor_SOURCES = \
	$(SOURCES_COMMON) \
	$(SOURCES_SDL) \
	app/trail_editor.hpp \
	app/trail_editor.cpp \
	trail_editor.cpp
trail_editor_LDADD = -lstree $(LIBS_SDL)
trail_editor_CXXFLAGS = \
	-DDATADIR=\"$(datadir)/antview/\" \
	-Wl,-rpath -Wl,$(prefix)/lib # ??

# Viewer
ant_viewer_SOURCES = \
	$(SOURCES_COMMON) \
	$(SOURCES_SDL) \
	app/ant_viewer.hpp \
	app/ant_viewer.cpp \
	primitives.hpp \
	primitives.cpp \
	ant_viewer.cpp
ant_viewer_LDADD = -lstree $(LIBS_SDL)
ant_viewer_CXXFLAGS = \
	-DDATADIR=\"$(datadir)/antview/\" \
	-Wl,-rpath -Wl,$(prefix)/lib # ??

# Evolution
evolve_ant_SOURCES = \
	$(SOURCES_COMMON) \
	evolve_ant.cpp \
	primitives.hpp \
	primitives.cpp
evolve_ant_LDADD = -lstree
evolve_ant_CXXFLAGS = -Wl,-rpath -Wl,$(prefix)/lib

# Tests

check_PROGRAMS = \
	test_trail_parser1 \
	test_ant1

test_trail_parser1_SOURCES = tests/trail_parser1.cpp \
	ant.hpp ant.cpp \
	trail_parser.hpp trail_parser.cpp

test_ant1_SOURCES = tests/ant1.cpp \
	ant.hpp ant.cpp \
	primitives.hpp primitives.cpp \
	trail_parser.hpp trail_parser.cpp
test_ant1_LDADD = -lstree
test_ant1_CXXFLAGS = -Wl,-rpath -Wl,$(prefix)/lib # ??
