BUILD_DIR=build
EXECUTABLE=mekus

SRC=./src
INCLUDES=./include

CC=gcc
# optimize compile time
OPTS=-O0
# flags for .h file dependencies
HFLAGS=-MP -MD

# compose all flags
CFLAGS=-g -Wall $(foreach DIR,$(INCLUDES),-I$(DIR)) $(OPTS) $(HFLAGS)

# generate a list of all .c files
SOURCES=$(foreach D,$(SRC),$(wildcard $(D)/*.c))
# .o
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))
# .d (gcc generates them)
DEPFILES=$(patsubst %.c,%.d,$(notdir $(SOURCES)))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^

$(BUILD_DIR)/%.o:$(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(EXECUTABLE) $(BUILD_DIR)

-include $(DEPFILES)
.PHONY: all clean

