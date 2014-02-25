CXX = g++ #-3.4.3#-4.0.2
DEBUG = #-pg #-g #-ggdb
WARNING = -Wall -W -Wno-unused-parameter -Wno-deprecated #-w
OPTIMIZER = -O6 #-finline-limit=1000

CXXFLAGS = $(DEBUG) $(WARNING) $(OPTIMIZER) 
# LIBS = /lib/libboost_regex-mt.dll.a /lib/libboost_iostreams-mt.dll.a
# LIBS = /usr/lib/libboost_regex-mt.a /usr/lib/libboost_iostreams-mt.a #ubuntu machines
LIBS = /usr/lib64/libboost_regex-mt.a /usr/lib64/libboost_iostreams-mt.a
LINK = -lboost_regex-mt -lboost_iostreams-mt

#CCOPTS   = -c -O
#
#SRC = main.cpp
#DEP = $(SRC:%.cpp=%.d) 
#main: main.o
#	$(CXX) $(LINK) -o $@ $^ $(LIBS)
#include $(DEP)
#
#.o:
#	g++ $(CCOPTS) $<
#	
#Setting: 
#	g++ -c -O Setting.cpp


clean:
	rm -f main *.o *.exe


build_jeppe: jeppemain.cpp 
	g++ -Wno-deprecated $(LINK) jeppemain.cpp -o jeppemain $(LIBS)	
	

#%.d: %.cpp
#	$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $< \
#                      | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
#                      [ -s $@ ] || rm -f $@'


# Compile IndexingCH.cpp
build_index: IndexingCH.cpp
	g++ -Wno-deprecated $(LINK) IndexingCH.cpp -o buildCHindex $(LIBS)

# Compile QueryingCH.cpp -- example SP file
build_query: QueryingCH.cpp
	g++ -Wno-deprecated $(LINK) QueryingCH.cpp -o QueryingCH $(LIBS)

# Create node ordering file for NY
# 
# -x 190 -y 1 -e 600 -p 1000 -k 1,3.3,2,10,3,10,5
# The above arguments specify the parameters of one of the CH heuristics which is called "EOS1235"
# "EOS1235" has the minimum contraction and node ordering time
# Example command for IndexingCH -- THIS WONT WORK ANYMORE, since it now reads .cedge files, not .ddsg files
#./buildCHindex -s -p -f data/NY_t_0.ddsg -o hcn/NY_t_0-EOS1235.hcn -l hcn/NY_t_0-EOS1235.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_NY:
	./buildCHindex -s -p -f USA-road-d.NY.ddsg -o USA-road-d.NY.hcn -l USA-road-d.NY.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5
	./buildCHindex -s -p -f USA-road-t.NY.ddsg -o USA-road-t.NY.hcn -l USA-road-d.NY.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_COL:
	./buildCHindex -s -p -f USA-road-d.COL.ddsg -o USA-road-d.COL.hcn -l USA-road-d.COL.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5
	./buildCHindex -s -p -f USA-road-t.COL.ddsg -o USA-road-t.COL.hcn -l USA-road-d.COL.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_USA:
	./buildCHindex -s -p -f USA-road-d.USA.ddsg -o USA-road-d.USA.hcn -l USA-road-d.USA.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5
	./buildCHindex -s -p -f USA-road-t.USA.ddsg -o USA-road-t.USA.hcn -l USA-road-d.USA.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_CTR:
	./buildCHindex -s -p -f USA-road-d.CTR.ddsg -o USA-road-d.CTR.hcn -l USA-road-d.CTR.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5
	./buildCHindex -s -p -f USA-road-t.CTR.ddsg -o USA-road-t.CTR.hcn -l USA-road-d.CTR.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_W:
	./buildCHindex -s -p -f USA-road-d.W.ddsg -o USA-road-d.W.hcn -l USA-road-d.W.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5
	./buildCHindex -s -p -f USA-road-t.W.ddsg -o USA-road-t.W.hcn -l USA-road-d.W.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_AAL:
	./buildCHindex -s -p -f aalborgNEW.ddsg -o aalborgNEW.hcn -l aalborgNEW.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

build_hcn_BEI:
	./buildCHindex -s -p -f beijingNEW.ddsg -o beijingNEW.hcn -l beijingNEW.log -x 190 -y 1 -e 600 -k 1,3.3,2,10,3,10,5

	


# Example command for QueryingCH

run_query_NY:
	./QueryingCH -f USA-road-d.NY.ddsg -h USA-road-d.NY.hcn -k 1,3.3,2,10,3,10,5

run_query_COL:
	./QueryingCH -f USA-road-d.COL.ddsg -h USA-road-d.COL.hcn -k 1,3.3,2,10,3,10,5
