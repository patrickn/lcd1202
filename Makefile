
CPPFLAGS=-Wall -lstdc++


all: lcd1202

main.o: main.cpp
	g++ ${CPPFLAGS} -c main.cpp

LCD1202.o: LCD1202.cpp
	g++ ${CPPFLAGS} -c LCD1202.cpp

lcd1202: main.o LCD1202.o
	g++ -o lcd1202 LCD1202.o main.o

clean:
	rm -f *.o lcd1202
