AM_LDFLAGS = @SDL_LIBS@
AM_CXXFLAGS = -Wall `@SDL_CONFIG@ --cflags` -I../include/

all: theme1@THEMEEXT@ gtk@THEMEEXT@ industrial@THEMEEXT@

%@THEMEEXT@: %.o ../src/.libs/libaedGUI.la
	g++ $(AM_CXXFLAGS) -shared -fPIC $< -o $@ -Wl,-rpath,../src/.libs/ -L../src/.libs/ -laedGUI $(AM_LDFLAGS)

%.o: %.cpp ../include/*h
	 g++ $(AM_CXXFLAGS) -c -fPIC $< -o $@

.PRECIOUS: theme1.o gtk.o industrial.o

EXTRA_DIST = theme1.cpp gtk.cpp industrial.cpp
