GCC ?=g++
AR ?=ar

OBJ_DIR=./obj
LIB_DIR=./lib

CXX_FLAGS +=--std=c++17 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -I../
AR_FLAGS +=rcs

OBJECTS =${OBJ_DIR}/import_sorter.o
LIBS =${LIB_DIR}/libisort.a
EXE =../isort-cpp

all: ${LIBS} ${EXE}

clean:
	rm -Rf ${OBJ_DIR} ${LIB_DIR} ${EXE}

${OBJ_DIR}:
	mkdir -p $@

${LIB_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc ${OBJ_DIR} $(wildcard $*.h)
	$(GCC) ${CXX_FLAGS} -c -o $@ $<

${LIB_DIR}/%.a: ${LIB_DIR} ${OBJECTS}
	$(AR) ${AR_FLAGS} $@ ${OBJECTS}

${EXE}: main.cc ${LIBS}
	$(GCC) ${CXX_FLAGS} -o ${EXE} $< ${LIBS}
