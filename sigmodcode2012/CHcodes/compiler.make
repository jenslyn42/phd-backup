CXX = g++ #-3.4.3#-4.0.2
DEBUG = #-pg #-g #-ggdb
WARNING = -Wall -W -Wno-unused-parameter -Wno-deprecated #-w
OPTIMIZER = -O6 #-finline-limit=1000

CXXFLAGS = $(DEBUG) $(WARNING) $(OPTIMIZER)
LIBS = /usr/lib64/libboost_regex-mt.a /usr/lib64/libboost_iostreams-mt.a
LINK = -lboost_regex-mt -lboost_iostreams-mt
