#
# $Id$
#

ROOT_DIR = ./
INIT_LIST_V1_PARENT_DIR = /Developer/SDKs/MacOSX10.7.sdk/usr/include/c++/

CLANG_CXX = /Users/petergoodman/Code/build/llvm/Debug+Asserts/bin/clang++
INTEL_CXX = icpc
GNU_CXX = g++

DEFAULT_CXX = ${CLANG_CXX}
DEFAULT_CC = /Users/petergoodman/Code/build/llvm/Debug+Asserts/bin/clang

CXX = ${DEFAULT_CXX}
CC = ${DEFAULT_CC}

CXX_FEATURES = -fno-rtti -fno-exceptions -fstrict-aliasing
CXX_WARN_FLAGS = -Wall -Werror -Wno-unused-function -Wno-variadic-macros 
CXX_WARN_FLAGS += -Wcast-qual -Wno-format-security
CXX_FLAGS = -O0 -g -ansi -I${ROOT_DIR} -std=c++0x
LD_FLAGS =

CC_FEATURES = -fstrict-aliasing
CC_WARN_FLAGS = -Wall -Werror -Wno-unused-function 
CC_WARN_FLAGS += -Wcast-qual -Wno-format-security
CC_FLAGS = -O0 -g -ansi -I${ROOT_DIR} -std=c99

GNU_COMPATIBLE_FLAGS = -pedantic -pedantic-errors -Wextra -Wcast-align -Wno-long-long

# are we compiling with the g++?
ifeq (${CXX}, ${GNU_CXX})
	#CXX_FEATURES += -flto
	CXX_FEATURES += -fno-stack-protector
	CXX_WARN_FLAGS += -Wshadow -Wpointer-arith \
				      -Wwrite-strings \
				      -Wfloat-equal -Wconversion -Wredundant-decls \
    				  -Wvolatile-register-var \
    				  -Wstack-protector \
    				  -Wstrict-aliasing=2 \
    				  -Wold-style-cast \
    				  -Wuninitialized
endif

# are we compiling with icc?
ifeq (${CXX}, ${INTEL_CXX})
	GNU_COMPATIBLE_FLAGS = 
	CXX_FEATURES += -fno-stack-protector
	CXX_WARN_FLAGS = -diag-disable 279
	CXX_FLAGS += -Kc++ -Wall -Werror -wd981 -ansi-alias
	LD_FLAGS += -lstdc++
endif

# are we compiling with clang++?
ifeq (${CXX}, ${CLANG_CXX})
	CXX_FEATURES += -fcatch-undefined-behavior -finline-functions
	CXX_WARN_FLAGS += -Winline
endif

CXX_FLAGS += ${CXX_WARN_FLAGS} ${CXX_FEATURES} ${GNU_COMPATIBLE_FLAGS}
CC_FLAGS += ${CC_WARN_FLAGS} ${CC_FEATURES} ${GNU_COMPATIBLE_FLAGS}

OBJS = 

OBJS += bin/deps/openbsd/realpath.o
OBJS += bin/deps/openbsd/strcasecmp.o
OBJS += bin/deps/openbsd/strdup.o
OBJS += bin/deps/openbsd/strlcat.o
OBJS += bin/deps/openbsd/strlcpy.o

OBJS += bin/lib/utf8/codepoint.o
OBJS += bin/lib/utf8/decoder.o 
OBJS += bin/lib/support/byte_reader.o
OBJS += bin/lib/support/cstring_reader.o
OBJS += bin/lib/io/file.o
OBJS += bin/lib/io/message.o
OBJS += bin/lib/io/cwd.o
OBJS += bin/lib/io/real_path.o
OBJS += bin/lib/io/line_highlight.o
OBJS += bin/lib/cstring.o
OBJS += bin/lib/diagnostic_context.o
OBJS += bin/lib/lexer.o
OBJS += bin/lib/token.o
OBJS += bin/lib/tokenizer.o
OBJS += bin/lib/token_stream.o

OBJS += bin/deps/libdatrie/string.o
OBJS += bin/deps/libdatrie/alpha-map.o
OBJS += bin/deps/libdatrie/darray.o
OBJS += bin/deps/libdatrie/tail.o
OBJS += bin/deps/libdatrie/trie.o

OBJS += bin/deps/linenoise/linenoise.o

OBJS += bin/lib/support/name_map.o
OBJS += bin/lib/type.o
OBJS += bin/lib/type_system.o
OBJS += bin/lib/symbol_table.o
OBJS += bin/lib/parser.o

OBJS += bin/main.o 
OUT = bin/xy

all: ${OBJS}
	${CXX} ${LD_FLAGS} ${OBJS} -o ${OUT}

bin/%.o: xy/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

bin/lib/%.o: xy/lib/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

bin/lib/utf8/%.o: xy/lib/utf8/%.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@
	
bin/deps/libdatrie/%.o: xy/deps/libdatrie/%.c
	${CC} ${CC_FLAGS} -c $< -o $@

bin/deps/libdatrie/%.o: xy/deps/libdatrie/%.c
	${CC} ${CC_FLAGS} -c $< -o $@

bin/deps/linenoise/%.o: xy/deps/linenoise/%.c
	${CC} ${CC_FLAGS} -c $< -o $@

bin/deps/openbsd/%.o: xy/deps/openbsd/%.c
	${CC} ${CC_FLAGS} -c $< -o $@

install:
	-mkdir bin
	-mkdir bin/lib
	-mkdir bin/lib/utf8
	-mkdir bin/lib/io
	-mkdir bin/lib/support
	-mkdir bin/deps
	-mkdir bin/deps/libdatrie
	-mkdir bin/deps/linenoise
	-mkdir bin/deps/openbsd

clean:
	-rm -f bin/*.o
	-rm -f bin/lib/*.o
	-rm -f bin/lib/utf8/*.o
	-rm -f bin/lib/io/*.o
	-rm -f bin/lib/support/*.o
	-rm -f bin/deps/libdatrie/*.o
	-rm -f bin/deps/linenoise/*.o
	-rm -f bin/deps/openbsd/*.o
