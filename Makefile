include Makefile.arch

SOURCES=$(wildcard ./src/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
LIB=./src/libRAPIDO.so

$(LIB): $(OBJECTS) 
	$(LD) $(LDFLAGS) $(SOFLAGS) $(OBJECTS) $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@

%.o:	%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ -fno-var-tracking

all: $(LIB) 
clean:
	rm -f ./src/*.o \
	rm -f ./src/*.d \
	rm -f ./src/*.so \
