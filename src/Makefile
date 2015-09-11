SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
INCLUDES = -I. -I/usr/include/SDL
LDFLAGS = $(shell pkg-config --libs lua5.1) -llua5.1 -lGL -lGLU -lglut -lGLEW -lSDL -lSDL_mixer -lpng -llua5.1 -lpthread
CPPFLAGS = $(shell pkg-config --cflags lua5.1)
CXXFLAGS = $(CPPFLAGS) -W -Wall -g
CXX = g++
MAIN = dungeon

all: $(MAIN)

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS)
	@echo Creating $@...
	@$(CXX) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(INCLUDES) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	@echo Building $@...
	@set -e; $(CC) -M $(CPPFLAGS) $(INCLUDES) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPENDS)