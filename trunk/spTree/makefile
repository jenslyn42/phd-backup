#			Linux 
#CC       = g++
#CCOPTS   = -c -I/usr/include/X11R5 -I/usr/X11R6/include -I/usr/include/Motif1.2 -g -DLINUX
#LINK     = g++
#LINKOPTS = -static -L/usr/lib/Motif1.2 -L/usr/lib/X11R5 -lm

#			HP-UX
CC       = g++
CCOPTS   = -c -O
LINK     = g++
LINKOPTS = -lm

PACK = spTreeRG.o


.cc.o:
	$(CC) $(CCOPTS) $<

all: main

spTreeRG.o: spTreeRG.cpp spTreeRG.h

main: main.cpp $(PACK) 
	$(LINK) -o mains main.cpp $(PACK) $(LINKOPTS)


# main: main.cpp $(PACK) 
# $(LINK) -O2 -o mains main.cpp $(PACK) $(LINKOPTS)
	
clean:
	rm *.o mains *~
