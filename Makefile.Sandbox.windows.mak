DIR := $(subst /,\,${CURDIR})
BUILD_DIR := output\bin
OBJ_DIR := output\obj

ASSEMBLY := Sandbox
EXTENSION := .exe
COMPILER_FLAGS := -g -MD -Wno-vla -Wno-missing-braces -fdeclspec #-fPIC -Werror=vla
INCLUDE_FLAGS := -IDubhe\src -ISandbox\src
LINKER_FLAGS := -g -lDubhe -L$(BUILD_DIR) #-L$(OBJ_DIR)\Dubhe -Wl,-rpath,.
DEFINES := -D_DEBUG -DDIMPORT

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRC_FILES := $(call rwildcard,$(ASSEMBLY)/,*.c) # Get all .c files
DIRECTORIES := \$(ASSEMBLY)\src $(subst $(DIR),,$(shell dir $(ASSEMBLY)\src /S /AD /B | findstr /i src)) # Get all directories under src.
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o) # Get all compiled .c.o objects for Sandbox

all: scaffold compile link

.PHONY: scaffold
scaffold: # create build directory
	@echo Scaffolding folder structure...
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # link
	@echo Linking $(ASSEMBLY)...
	@echo clang $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile: #compile .c files
	@echo Compiling...

.PHONY: clean
clean: # clean build directory
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)\$(ASSEMBLY)

$(OBJ_DIR)/%.c.o: %.c # compile .c to .c.o object
	@echo   $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)