include compiler.make

SRC = main.cpp
DEP = $(SRC:%.cpp=%.d) 

main: main.o
	$(CXX) $(LINK) -o $@ $^ $(LIBS)

clean:
	rm -f main main.o *.exe

%.d: %.cpp
	$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $< \
                      | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
                      [ -s $@ ] || rm -f $@'

# Create node ordering file for NY and BAY
# 
# -x 190 -y 1 -e 600 -p 1000 -k 1,3.3,2,10,3,10,5
# The above arguments specify the parameters of one of the CH heuristics which is called "EOS1235"
# "EOS1235" has the minimum contraction and node ordering time
build_hcn_NY: main 
	./main -s -p -f data/NY_t_0.ddsg -o hcn/NY_t_0-EOS1235.hcn -l hcn/NY_t_0-EOS1235.log -x 190 -y 1 -e 600 -p 1000 -k 1,3.3,2,10,3,10,5

build_hcn_BAY: main
	./main -s -p -f data/BAY_t_0.ddsg -o hcn/BAY_t_0-EOS1235.hcn -l hcn/BAY_t_0-EOS1235.log -x 190 -y 1 -e 600 -p 1000 -k 1,3.3,2,10,3,10,5

hcn/NY_t_0-EOS1235.hcn: 
	make build_hcn_NY

hcn/BAY_t_0-EOS1235.hcn:
	make build_hcn_BAY

# Compile QuerySample.cpp
build_query: QuerySample.cpp hcn/NY_t_0-EOS1235.hcn hcn/BAY_t_0-EOS1235.hcn
	g++ -Wno-deprecated $(LINK) QuerySample.cpp -o QuerySample $(LIBS)

# Run QuerySample.cpp with NY data set
run_query: build_query
	./QuerySample -f data/NY_t_0.ddsg -h hcn/NY_t_0-EOS1235.hcn -k 1,3.3,2,10,3,10,5
	
include $(DEP)
