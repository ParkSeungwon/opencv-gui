CXX = g++
OBJ = $(wildcard *.o)
TOBJ = $(wildcard *.t)
PKGS = opencv4 zbar libhangul python3-embed
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = #-Wl,-rpath=.
LDLIBS = $(shell pkg-config --libs $(PKGS)) -pthread


all : $(EXE)

../%.x : %.x $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

