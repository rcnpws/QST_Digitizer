CXX = g++
CXXFLAGS = -Wall -g -W -fPIC --std=c++11

ROOTINC  = `root-config --incdir`
ROOTLIBS = `root-config --libs`

PROGRAM = chkwave
SRCS    = wave.C
OBJS    = $(SRCS:%.C=%.o)

all: $(PROGRAM)

$(PROGRAM): $(SRCS) wave.h utils.h defwave.h
	$(CXX) -o $@ $(SRCS) $(CXXFLAGS) $(ROOTLIBS) -I $(ROOTINC) 

clean:
	rm -f $(PROGRAM); rm -rf $(PROGRAM).dSYM


