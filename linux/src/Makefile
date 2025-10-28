# CMDDIR = ..
# BINDIR=  $(CMDDIR)
BINDIR = ..
CC     = gcc
CFLAGS = -O
INCDIR = -I$(HOME)/lib/include 
GFLAGS = `gtk-config --cflags` `gtk-config --libs`
LIBS   = -lm
CERN    = -lgraflib -lgrafX11 -lpacklib -lkernlib -lnsl

# for using ROOT
USEROOT      := $(shell which root)

CXX           = g++
CXXFLAGS      = -O -Wall -fPIC -I. -IHistogram
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared

LIBS          = $(SYSLIBS) -pthread
GLIBS         = $(SYSLIBS) -pthread

ifneq ($(USEROOT),)

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS  := $(shell root-config --ldflags)
ROOTLIBS     := $(shell root-config --new --libs)
ROOTGLIBS    := $(shell root-config --new --glibs)
HASTHREAD    := $(shell root-config --has-thread)

CXXFLAGS     += $(ROOTCFLAGS) -D__USEROOT__
LDFLAGS      += $(ROOTLDFLAGS)

LIBS         += $(ROOTLIBS)
GLIBS        += $(ROOTGLIBS)

endif

offline: offline.cxx
	$(CXX) $^ -o $(BINDIR)/$@ $(CXXFLAGS)  $(ROOTGLIBS) -g

clean: 
	rm -f *~ *.o $(BINDIR)/offline
