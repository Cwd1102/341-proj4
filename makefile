CXX = g++
CXXFLAGS = -Wall -g

mytest: dealer.h dealer.cpp mytest.cpp
	$(CXX) $(CXXFLAGS) dealer.h dealer.cpp mytest.cpp -o mytest

clean:
	rm mytest

run:
	./mytest

val:
	valgrind --track-origins=yes -s --leak-check=full ./mytest
