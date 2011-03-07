AM_CXXFLAGS = -g -O2 `@SDL_CONFIG@ --cflags` -I../include/

TEST_PROGS = aedGuiTest@EXEEXT@ testsprite@EXEEXT@ gltest@EXEEXT@ menutest@EXEEXT@ xmltest@EXEEXT@

all:	$(TEST_PROGS)

clean:
	  rm -f *.o $(TEST_PROGS)

## bypass crappy libtool linking ...
aedGuiTest@EXEEXT@: main.o ../src/.libs/libaedGUI.la
	$(CXX) $(AM_CXXFLAGS) $< -o $@ -Wl,-rpath,../src/.libs/ \
	-L../src/.libs/ -laedGUI @SDL_LIBS@

main.o: main.cpp main.h ../include/*h
	$(CXX) $(AM_CXXFLAGS) -c $< -o $@

testsprite@EXEEXT@: testsprite.o ../src/.libs/libaedGUI.la
	$(CXX) $(AM_CXXFLAGS) $< -o $@ -Wl,-rpath,../src/.libs/ \
	-L../src/.libs/ -laedGUI @SDL_LIBS@

testsprite.o: testsprite.cpp ../include/*h
	$(CXX) $(AM_CXXFLAGS) -c $< -o $@

menutest@EXEEXT@: menutest.o ../src/.libs/libaedGUI.la
	$(CXX) $(AM_CXXFLAGS) $< -o $@ -Wl,-rpath,../src/.libs/ \
	-L../src/.libs/ -laedGUI @SDL_LIBS@

menutest.o: menutest.cpp ../include/*h
	$(CXX) $(AM_CXXFLAGS) -c $< -o $@

xmltest@EXEEXT@: xmltest.o ../src/.libs/libaedGUI.la
	$(CXX) $(AM_CXXFLAGS) $< -o $@ -Wl,-rpath,../src/.libs/ \
	-L../src/.libs/ -laedGUI @SDL_LIBS@

xmltest.o: xmltest.cpp ../include/*h
	$(CXX) $(AM_CXXFLAGS) -c $< -o $@

gltest@EXEEXT@: gltest.o ../src/.libs/libaedGUI.la
	$(CXX) $(AM_CXXFLAGS) gltest.o -o gltest@EXEEXT@ \
	-Wl,-rpath,../src/.libs/ \
	-L../src/.libs/ -laedGUI @SDL_LIBS@ @GL_LIBS@

gltest.o: gltest.cpp
	$(CXX) $(AM_CXXFLAGS) -c gltest.cpp -o gltest.o

EXTRA_DIST = xmltest.cpp test1.xml menutest.cpp main.cpp main.h testsprite.cpp test.bmp Vera.ttf font.h icon.bmp gltest.cpp cursor.bmp Crate.bmp
