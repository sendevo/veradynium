CPP      = g++
CFLAGS   = -Wall -pthread
LDFLAGS  = -lgsl -lgslcblas -lm

SRCDIR   = src
OBJDIR   = obj
INCDIR   = include

TARGET   = solver

SOURCES  = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

HEADERS  = $(wildcard $(INCDIR)/*.hpp)

INCLUDES = -I$(INCDIR)

ifdef VERBOSE
    CFLAGS += -DVERBOSE=true
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "$(TARGET) built successfully"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
