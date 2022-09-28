CXX = g++
PKGS = opencv4 libhangul spdlog
CPPFLAGS = -I../ -pthread
CXXFLAGS = -g -c -w -std=c++20 -fmax-errors=1 $(shell pkg-config --cflags $(PKGS) )
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ) $(OOBJ)

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

../obj/%.$(DIR).x : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
