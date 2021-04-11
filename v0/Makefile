CXX	     = clang++
CXXFLAGS = -I. -gfull -O3 -DNDEBUG
LDFLAGS	 = -fuse-ld=lld

SOURCES      = Main.cpp HashTable.cpp
OBJS		 = $(SOURCES:.cpp=.o)
EXECUTABLE	 = hash_table

.DEFAULT_GOAL: all
.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJS)
