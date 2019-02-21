SOURCES=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SOURCES))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++11

main: $(OBJS)
	g++ $(CPPFLAGS) -o main $(OBJS)
%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

clean:
	rm main *~ *.o
