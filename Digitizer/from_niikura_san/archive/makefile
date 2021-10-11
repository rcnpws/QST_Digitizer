# CXX = g++
# CXXFLAGS = -Wall -g -W -fPIC --std=c++11
CXX      = `root-config --cxx`
CXXFLAGS = `root-config --cflags`
CXXFLAGS2 = -Wl,-rpath,/opt/root/pro/lib

ROOTINC  = `root-config --incdir`
ROOTLIBS = `root-config --libs`

PROGRAM = analysis
SRCS    = analysis.C
OBJS    = $(SRCS:%.C=%.o)

all: $(PROGRAM)

$(PROGRAM): $(SRCS) analysis.h utils.h wave.h ge.h
	$(CXX) -o $@ $(SRCS) $(CXXFLAGS) $(CXXFLAGS2) $(ROOTLIBS) -I $(ROOTINC) 

clean:
	rm -f $(PROGRAM); rm -rf $(PROGRAM).dSYM


