
CPPUNIT_PATH=/usr
CPPFLAGS=-Wall -lstdc++


all: lcd1202
unittest: lcd1202test

main.o: main.cpp
	g++ ${CPPFLAGS} -c main.cpp

LCD1202.o: LCD1202.cpp
	g++ ${CPPFLAGS} -c LCD1202.cpp

lcd1202: main.o LCD1202.o
	g++ -o lcd1202 LCD1202.o main.o


test.o: test.cpp
	g++ ${CPPFLAGS} -c test.cpp

LCD1202Test.o: LCD1202Test.cpp
	g++ ${CPPFLAGS} -c LCD1202Test.cpp

lcd1202test: test.o LCD1202Test.o LCD1202.o
	g++ -Wall -o lcd1202test test.o LCD1202Test.o LCD1202.o -L${CPPUNIT_PATH}/lib -lcppunit -ldl

clean:
	rm -f *.o lcd1202 lcd1202test
