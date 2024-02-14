LDFLAGS= -lncurses -ltinfo

CXX= g++

all: main clean

main: main.o asciiGL.o
	$(CXX) main.o asciiGL.o -o main $(LDFLAGS)

debug: CXXFLAGS += -g
debug: main

clean:
	rm *.o
