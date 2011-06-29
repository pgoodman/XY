#
# $Id$
#

ROOT_DIR = ./

DEFAULT_CXX = clang++
CXX = ${DEFAULT_CXX}
CXX_FEATURES = -fno-rtti -fno-exceptions -fstrict-aliasing
CXX_WARN_FLAGS += -Wall -Werror -Wno-unused-function 
CXX_WARN_FLAGS += -Wcast-qual
CXX_FLAGS = -O2 -g -ansi -I${ROOT_DIR} -std=c++0x
LD_FLAGS =

GNU_COMPATIBLE_FLAGS = -pedantic -pedantic-errors -Wextra -Wcast-align -Wno-long-long 

# are we compiling with the g++?
ifeq (${CXX}, g++)
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
ifeq (${CXX}, icpc)
	GNU_COMPATIBLE_FLAGS = 
	CXX_FEATURES += -fno-stack-protector
	CXX_WARN_FLAGS = -diag-disable 279
	CXX_FLAGS += -Kc++ -Wall -Werror -wd981 -ansi-alias
	LD_FLAGS += -lstdc++
endif

# are we compiling with clang++?
ifeq (${CXX}, clang++)
	CXX_FEATURES += -fcatch-undefined-behavior -finline-functions
	CXX_WARN_FLAGS += -Winline
endif

CXX_FLAGS += ${CXX_WARN_FLAGS} ${CXX_FEATURES} ${GNU_COMPATIBLE_FLAGS}
OBJS = 
OBJS += bin/lib/utf8/codepoint.o
OBJS += bin/lib/utf8/decoder.o 
OBJS += bin/lib/io/file.o
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

clean:
	-rm ${OUT}
	-rm -rf bin/*.o
	-rm -rf bin/lib/*.o
	-rm -rf bin/lib/utf8/*.o
	-rm -rf bin/lib/io/*.o

