GCC ?=g++
AR ?=ar

OBJ_DIR=./obj
PROD_LIB_DIR=../src/lib

CXX_FLAGS +=--std=c++17 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -I../src -L${PROD_LIB_DIR}
LD_FLAGS +=-lgtest_main -lgtest -lgmock -lpthread

OBJECTS =${OBJ_DIR}/import_sorter_tests.o
LIBS =${PROD_LIB_DIR}/libisort.a
EXE =unittests

${EXE}: ${OBJECTS} ${LIBS}
	$(GCC) ${CXX_FLAGS} ${LD_FLAGS} -o $@ $< ${LIBS}

clean:
	rm -Rf ${OBJ_DIR} ${EXE}

${OBJ_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc ${OBJ_DIR} $(wildcard $*.h)
	$(GCC) ${CXX_FLAGS} -c -o $@ $<
