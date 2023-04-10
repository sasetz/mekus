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
CFLAGS=-g -Wall -Wextra $(foreach DIR,$(INCLUDES),-I$(DIR)) $(OPTS) $(HFLAGS)

# generate a list of all .c files
SOURCES=$(foreach FILE,$(SRC),$(wildcard $(DIR)/*.c))
# .o
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(CFILES))
# .d (gcc generates them)
DEPFILES=$(patsubst %.c,%.d,$(CFILES))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^

%.o:$(BUILD_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(EXECUTABLE) $(BUILD_DIR)

-include $(DEPFILES)
.PHONY: all clean

