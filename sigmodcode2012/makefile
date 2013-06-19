CC       = g++
CCOPTS   = -c
LINK     = -lboost_regex-mt -lboost_iostreams-mt  #-std=c++11 
LIBS = /usr/lib64/libboost_regex-mt.a /usr/lib64/libboost_iostreams-mt.a
LINKOPTS = -lm

WARNING = -Wall -W -Wno-unused-parameter -Wno-deprecated #-w
OPTIMIZER = -O6 #-finline-limit=1000

CXXFLAGS = $(OPTIMIZER) #$(WARNING) 

#for CH
SRC = CHcodes/main.cpp
DEP = $(SRC:%.cpp=%.d)

PACK = Setting.o RoadGraph.o Cache.o Probcache.o Competitors.o ProtoStudy.o 


%.o: %.cpp
	$(CC) -Wno-deprecated $(CXXFLAGS) $(CCOPTS) $< -o $@

all: main


Setting.o: Setting.cpp Setting.h

RoadGraph.o: RoadGraph.cpp RoadGraph.h Setting.h

Cache.o: Cache.cpp Cache.h RoadGraph.h Setting.h

Probcache.o: Probcache.cpp Probcache.h Cache.h RoadGraph.h Setting.h

Competitors.o: Competitors.cpp Competitors.h Cache.h RoadGraph.h Setting.h

ProtoStudy.o: ProtoStudy.cpp ProtoStudy.h Cache.h RoadGraph.h Setting.h



main: main.cpp $(PACK) 
	$(CC) $(LINK) -Wno-deprecated -o mains main.cpp $(PACK) $(LINKOPTS) $(LIBS)


# main: main.cpp $(PACK) 
# $(LINK) -O2 -o mains main.cpp $(PACK) $(LINKOPTS)
	
clean:
	rm *.o

man: main.cpp Setting.cpp Setting.h RoadGraph.cpp RoadGraph.h Cache.cpp Cache.h Probcache.cpp Probcache.h Competitors.cpp Competitors.h ProtoStudy.cpp ProtoStudy.h 
	$(CC) $(LINK) -Wno-deprecated -o mains main.cpp Setting.cpp Setting.h RoadGraph.cpp RoadGraph.h Cache.cpp Cache.h Probcache.cpp Probcache.h Competitors.cpp Competitors.h ProtoStudy.cpp ProtoStudy.h  $(LINKOPTS) $(LIBS)


include $(DEP)