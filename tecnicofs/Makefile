#                   
#                     Projeto de Sistemas Operativos 2021-22
#                         1º exercício LEIC-A/LEIC-T/LETI 
#                        
#                                Grupo 17 - LETI
#                    Pedro Alexandre Delgado Claro ist198960
#                   Edson Fernando Cabral da Veiga ist1100731
# 
 

# Makefile
# Sistemas Operativos, DEI/IST/ULisboa 2021-22
# This makefile should be run from the *root* of the project

CC ?= gcc
LD ?= gcc

# space separated list of directories with header files
INCLUDE_DIRS := fs .
# this creates a space separated list of -I<dir> where <dir> is each of the values in INCLUDE_DIRS
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

SOURCES  := $(wildcard */*.c)
HEADERS  := $(wildcard */*.h)
OBJECTS  := $(SOURCES:.c=.o)
TARGET_EXECS := tests/ourtest_3threads_1W2R tests/ourtest_4threads_2W2R tests/ourtest_3threads_2W1R

# VPATH is a variable used by Makefile which finds *sources* and makes them available throughout the codebase
# vpath %.h <DIR> tells make to look for header files in <DIR>
vpath # clears VPATH
vpath %.h $(INCLUDE_DIRS)

CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L
CFLAGS += $(INCLUDES)

# Warnings
CFLAGS += -fdiagnostics-color=always -Wall -Werror -Wextra -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wsign-conversion -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# Warning suppressions
CFLAGS += -Wno-sign-compare

LDFLAGS += -lpthread -pthread
# optional debug symbols: run make DEBUG=no to deactivate them
ifneq ($(strip $(DEBUG)), no)
  CFLAGS += -g
  CFLAGS += -fsanitize=address
  LDFLAGS += -fsanitize=address
  CXXFLAGS = -fsanitize=address
  
endif

# optional O3 optimization symbols: run make OPTIM=no to deactivate them
ifeq ($(strip $(OPTIM)), no)
  CFLAGS += -O0
else
  CFLAGS += -O3
endif

# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean depend fmt

all: $(TARGET_EXECS)


# The following target can be used to invoke clang-format on all the source and header
# files. clang-format is a tool to format the source code based on the style specified 
# in the file '.clang-format'.
# More info available here: https://clang.llvm.org/docs/ClangFormat.html

# The $^ keyword is used in Makefile to refer to the right part of the ":" in the 
# enclosing rule. See https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

fmt: $(SOURCES) $(HEADERS)
	clang-format -i $^



# Note the lack of a rule.
# make uses a set of default rules, one of which compiles C binaries
# the CC, LD, CFLAGS and LDFLAGS are used in this rule
tests/ourtest_3threads_2W1R: tests/ourtest_3threads_2W1R.o fs/operations.o fs/state.o fs/threads_api.o
tests/ourtest_4threads_2W2R: tests/ourtest_4threads_2W2R.o fs/operations.o fs/state.o fs/threads_api.o
tests/ourtest_3threads_1W2R: tests/ourtest_3threads_1W2R.o fs/operations.o fs/state.o fs/threads_api.o

test1:
	./tests/ourtest_3threads_2W1R

test2:
	./tests/ourtest_4threads_2W2R

test3:
	./tests/ourtest_3threads_1W2R

tests:
	./tests/ourtest_3threads_2W1R
	./tests/ourtest_4threads_2W2R
	./tests/ourtest_3threads_1W2R


clean:
	rm -f $(OBJECTS) $(TARGET_EXECS)


# This generates a dependency file, with some default dependencies gathered from the include tree
# The dependencies are gathered in the file autodep. You can find an example illustrating this GCC feature, without Makefile, at this URL: https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/MM
# Run `make depend` whenever you add new includes in your files
depend : $(SOURCES)
	$(CC) $(INCLUDES) -MM $^ > autodep
