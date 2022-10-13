CXX = g++
PKGS = opencv4 libhangul
CPPFLAGS = -I../ -pthread -DCVPLOT_HEADER_ONLY 
CXXFLAGS = -g -c -w -std=c++23 -fmax-errors=1 $(shell pkg-config --cflags $(PKGS) )
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ) $(OOBJ)

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

../obj/%.$(DIR).x : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
