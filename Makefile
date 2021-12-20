.PHONY: clean profile valgrind test

CC = gcc
CXX = g++
CFLAGS += -O2 -Wall
CFLAGS += -g
#CFLAGS += -pg
CFLAGS += -DTRACE
LDFLAGS = 
APPLICATION = acherond
SUPPLEMENTARY = ComplexParser.hpp GenericException.hpp kernel/sharedinfo.h Output.hpp Request.hpp Request_IP.hpp Request_Netlink.hpp Request_TCP.hpp Request_UDP.hpp Rule.hpp Rule_IP.hpp Rule_Netlink.hpp RuleSet.hpp Rule_TCP.hpp Rule_UDP.hpp StringParser.hpp Trace.hpp Types.hpp
OBJS = StringParser.o GenericException.o ComplexParser.o Rule.o Rule_IP.o Rule_IP6.o Rule_TCP.o Rule_UDP.o Rule_TCP6.o Rule_UDP6.o Rule_Netlink.o RuleSet.o Trace.o Output.o SystemResolver.o Logger.o

all: $(APPLICATION)

clean:
	rm -f  $(APPLICATION) $(OBJS)

profile: $(APPLICATION)
	./$(APPLICATION)
	gprof -b $(APPLICATION)

valgrind: $(APPLICATION)
	valgrind --leak-check=yes ./$(APPLICATION)

test: $(APPLICATION)
	./$(APPLICATION)

$(APPLICATION): $(APPLICATION).cpp $(SUPPLEMENTARY) $(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $(APPLICATION) $(APPLICATION).cpp $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<
