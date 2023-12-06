CXX = g++
CXXFLAGS = -Wall -g

driver: dealer.h dealer.cpp driver.cpp
	$(CXX) $(CXXFLAGS) dealer.h dealer.cpp driver.cpp -o driver

clean:
	rm driver

run:
	./driver

val:
	valgrind --track-origins=yes -s --leak-check=full ./driver
