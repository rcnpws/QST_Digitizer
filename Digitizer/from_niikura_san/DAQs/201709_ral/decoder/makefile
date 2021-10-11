CXX = g++
CXXFLAGS = -Wall -g -W -fPIC --std=c++11

ROOTINC  = `root-config --incdir`
ROOTLIBS = `root-config --libs`

PROGRAM = mktree
SRCS    = wave.C
OBJS    = $(SRCS:%.C=%.o)

all: $(PROGRAM)

$(PROGRAM): $(SRCS) utils.h detector.h histogram.h oscilloscope.h ge.h
	$(CXX) -o $@ $(SRCS) $(CXXFLAGS) $(ROOTLIBS) -I $(ROOTINC) 

clean:
	rm -f $(PROGRAM); rm -rf $(PROGRAM).dSYM


