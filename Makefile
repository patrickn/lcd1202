
CPPUNIT_PATH = /usr
CPPFLAGS     = -Wall -lstdc++

# Grab the version number from the latest tag
GIT     = /usr/bin/git
VERSION = $(shell ${GIT} describe --abbrev=4 HEAD)


all: version lcd1202
unittest: lcd1202test

main.o: main.cpp
	g++ ${CPPFLAGS} -c main.cpp

LCD1202.o: LCD1202.cpp LCD1202.h font.h
	g++ ${CPPFLAGS} -c LCD1202.cpp

lcd1202: main.o LCD1202.o
	g++ -o lcd1202 LCD1202.o main.o


test.o: test.cpp
	g++ ${CPPFLAGS} -c test.cpp

LCD1202Test.o: LCD1202Test.cpp LCD1202Test.h
	g++ ${CPPFLAGS} -c LCD1202Test.cpp

lcd1202test: test.o LCD1202Test.o LCD1202.o
	g++ -Wall -o lcd1202test test.o LCD1202Test.o LCD1202.o -L${CPPUNIT_PATH}/lib -lcppunit -ldl

version:
	@echo "const char* version = \""$(VERSION)"\";" > version.h

clean:
	rm -f *.o version.h lcd1202 lcd1202test
