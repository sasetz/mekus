BUILD_DIR=build
DEBUG_DIR=debug
EXECUTABLE=mekus
DEBUG_EXECUTABLE=mekus_gdb

SRC=./src
INCLUDES=./include

CC=gcc
# optimize compile time
OPTS=-O0
# flags for .h file dependencies
HFLAGS=-MP -MD

# compose all flags
CFLAGS=-Wall $(foreach DIR,$(INCLUDES),-I$(DIR)) $(OPTS) $(HFLAGS)
DFLAGS=-g -ggdb3 -Wall $(foreach DIR,$(INCLUDES),-I$(DIR)) $(OPTS) $(HFLAGS)

# generate a list of all .c files
SOURCES=$(foreach D,$(SRC),$(wildcard $(D)/*.c))
# .o
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))
# .d (gcc generates them)
DEPFILES=$(patsubst %.c,%.d,$(notdir $(SOURCES)))
# .o (debug)
DEBUG_OBJECTS=$(patsubst %.c,$(DEBUG_DIR)/%.o,$(notdir $(SOURCES)))

all: $(EXECUTABLE)

debug: $(DEBUG_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^

$(DEBUG_EXECUTABLE): $(DEBUG_OBJECTS)
	$(CC) -ggdb3 -o $@ $^

$(DEBUG_DIR)/%.o:$(SRC)/%.c
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(DFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(SRC)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(EXECUTABLE) $(BUILD_DIR)

-include $(DEPFILES)
.PHONY: all clean debug

