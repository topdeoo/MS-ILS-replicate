CXX := clang++
CXXFLAGS := -std=c++2a -Wall -Wextra -Werror -pedantic -ggdb -O
CXXFLAGS += -I../include
INCLUDE_DIR := ../include
SOURCE_DIR := ../src
EXE := checker hmecu

.PHONY: checker hmecu clean

checker: checker.o graph.o
	$(CXX) $(CXXFLAGS) -o checker checker.o graph.o

checker.o: checker.cpp
	$(CXX) $(CXXFLAGS) -c checker.cpp

hmecu: hmecu_test.o graph.o capmds.o
	$(CXX) $(CXXFLAGS) -o hmecu hmecu_test.o graph.o capmds.o

hmecu_test.o: hmecu_test.cpp
	$(CXX) $(CXXFLAGS) -c hmecu_test.cpp

graph.o: $(SOURCE_DIR)/graph.cpp $(INCLUDE_DIR)/graph.hpp
	$(CXX) $(CXXFLAGS) -c $(SOURCE_DIR)/graph.cpp

capmds.o: $(SOURCE_DIR)/capmds.cpp $(INCLUDE_DIR)/capmds.hpp
	$(CXX) $(CXXFLAGS) -c $(SOURCE_DIR)/capmds.cpp

clean:
	rm -f $(EXE) *.o
