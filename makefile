cCXX = g++
CXXFLAGS = -Wall -g

mytest: person.o hash.o mytest.cpp
	$(CXX) $(CXXFLAGS) person.o hash.o mytest.cpp -o mytest

hash.o: hash.h hash.cpp hash.o
	$(CXX) $(CXXFLAGS) -c person.o hash.cpp

person.o: person.h person.cpp
	$(CXX) $(CXXFLAGS) -c person.cpp

run:
	./mytest

