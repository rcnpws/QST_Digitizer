CXX = g++
CXXFLAGS = -Wall -g -W -fPIC --std=c++11

ROOTINC  = `root-config --incdir`
ROOTLIBS = `root-config --libs`

PROGRAM = evtbuild
SRCS    = evtbuild.C
OBJS    = $(SRCS:%.C=%.o)

all: $(PROGRAM)

$(PROGRAM): $(SRCS) BN0.h Ge0.h Ge1.h
	$(CXX) -o $@ $(SRCS) $(CXXFLAGS) $(ROOTLIBS) -I $(ROOTINC) 

clean:
	rm -f $(PROGRAM); rm -rf $(PROGRAM).dSYM


