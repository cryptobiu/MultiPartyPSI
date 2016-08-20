
BIN=bin
SRC=src
EXT=${SRC}/externals


# compiler settings
CC=g++
#COMPILER_OPTIONS=-O2
COMPILER_OPTIONS=-g3 -O2 #-fPIC -mavx -maes -mpclmul -DRDTSC -DTEST=AES128

DEBUG_OPTIONS=-g3 -ggdb #-Wall -Wextra

BATCH=

ARCHITECTURE = $(shell uname -m)
ifeq (${ARCHITECTURE},x86_64)
MIRACL_MAKE:=linux64
GNU_LIB_PATH:=x86_64
else
MIRACL_MAKE:=linux
GNU_LIB_PATH:=i386
endif

INCLUDE=-I..  -I/usr/include/glib-2.0/ -I/usr/lib/${GNU_LIB_PATH}-linux-gnu/glib-2.0/include


LIBRARIES=-lgmp -lgmpxx -lpthread  -L /usr/lib  -lssl -lcrypto -lglib-2.0
CFLAGS=

# directory for the Miracl submodule and library
MIRACL_LIB_DIR=${EXT}/miracl_lib
SOURCES_MIRACL=${EXT}/Miracl/*
OBJECTS_MIRACL=${MIRACL_LIB_DIR}/*.o
MIRACL_LIB=${EXT}/miracl_lib/miracl.a


all: miracl
	@echo "make all done."

%.o:%.cpp %.h
	${CC} $< ${COMPILER_OPTIONS} ${DEBUG_OPTIONS} -c ${INCLUDE} ${LIBRARIES} ${CFLAGS} ${BATCH} -o $@

# this will create a copy of the files in ${SOURCES_MIRACL} and its sub-directories and put them into ${MIRACL_LIB_DIR} without sub-directories, then compile it
miracl:	${MIRACL_LIB_DIR}/miracl.a

# copy Miracl files to a new directory (${CORE}/util/miracl_lib/), call the build script and delete everything except the archive, header and object files.
${MIRACL_LIB_DIR}/miracl.a: ${SOURCES_MIRACL}
	@find ${EXT}/Miracl/ -type f -exec cp '{}' ${EXT}/miracl_lib \;
	@cd ${EXT}/miracl_lib/; bash ${MIRACL_MAKE}; find . -type f -not -name '*.a' -not -name '*.h' -not -name '*.o' -not -name '.git*'| xargs rm

# only clean example objects, test object and binaries
clean:
	rm -f ${OBJECTS_MIRACL} ${MIRACL_LIB_DIR}/*.a
