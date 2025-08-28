CPP      = g++
CFLAGS   = -Wall -pthread
LDFLAGS  = -lgsl -lgslcblas -lm

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
INCDIR   = include

# Find all entry points (*_main.cpp)
MAIN_SOURCES = $(wildcard $(SRCDIR)/*_main.cpp)
TARGETS      = $(patsubst $(SRCDIR)/%_main.cpp,$(BINDIR)/%,$(MAIN_SOURCES))

# Common sources = everything except *_main.cpp
COMMON_SOURCES = $(filter-out $(MAIN_SOURCES),$(wildcard $(SRCDIR)/*.cpp))
COMMON_OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(COMMON_SOURCES))

HEADERS  = $(wildcard $(INCDIR)/*.hpp)
INCLUDES = -I$(INCDIR)

ifdef VERBOSE
    CFLAGS += -DVERBOSE=true
endif

all: $(TARGETS)

# Generic rule: each binary depends on common objects + its own main
$(BINDIR)/%: $(COMMON_OBJECTS) $(OBJDIR)/%_main.o
	@mkdir -p $(@D)
	@echo "Linking $@..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "$@ built successfully"

# Compile rule for all .cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
