NAME := NodeModuleCleaner

SOURCE_DIR := src
BINARY_DIR := bin
OBJECT_DIR := obj
EXE := $(BINARY_DIR)/$(NAME)


CC := gcc
CFLAGS := -std=c89 			# Conform to the ISO 1989 C standard

ifeq ($(BUILD_MODE),DEBUG)
	CFLAGS += -g				# Enable debugging
endif

ifeq ($(BUILD_MODE),RELEASE)
	CFLAGS += -Wall	 			# Enable most warning messages
	CFLAGS += -Wextra			# Print extra (possibly unwanted) warnings
	CFLAGS += -pedantic			# Evaluates more warnings
	CFLAGS += -Werror			# Everything is an error
endif

# Files
SOURCES := $(wildcard $(SOURCE_DIR)/*.c) $(wildcard $(SOURCE_DIR)/**/*.c)
HEADERS := $(wildcard $(SOURCE_DIR)/*.h) $(wildcard $(SOURCE_DIR)/**/*.h)
OBJECTS := $(addprefix $(OBJECT_DIR)/, $(subst $(SOURCE_DIR)/, , $(SOURCES:%.c=%.o)))

# Targets ---------------------------------------------------------------------

default: all

all: clean $(EXE)

build: $(EXE)

# Remove all of the build files
clean:
	@ rm -rf $(BINARY_DIR)
	@ rm -rf $(OBJECT_DIR)

# Link the interpreter.
$(EXE): $(OBJECTS)
	@ echo "*" $(CC) -o $@ $(OBJECTS) $(CFLAGS)
	@ mkdir -p "./$(BINARY_DIR)"
	@ $(CC) -o $@ $(OBJECTS) $(CFLAGS)

# Compile object files.
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@ echo "*" $(CC) -c -o $@ $< $(CFLAGS)
	@ mkdir -p $(addprefix ./, $(dir $(OBJECTS)))
	@ $(CC) -c -o $@ $< $(CFLAGS)