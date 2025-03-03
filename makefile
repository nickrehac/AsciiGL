LDFLAGS= -lncurses -ltinfo

CXX= g++

all: main clean

main: main.o asciiGL.o
	$(CXX) main.o asciiGL.o -o main $(LDFLAGS)

debug: CXXFLAGS += -g
debug: main

release: CXXFLAGS += -O3
release: main

clean:
	rm *.o
