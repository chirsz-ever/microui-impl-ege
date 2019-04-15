SRC=./src
OBJ=./obj
BIN=./bin
CXX=g++
CC=gcc
CXXFLAGS=-std=c++11 -I./src -Wall 
CFLAGS=-std=c99 -I./src -Wall 
LINK_FLAGS=-L./src -lgraphics64 -lkernel32 -luser32 -lgdi32 -lole32 -loleaut32 -limm32 -lmsimg32 -luuid -mwindows
EXE_SUFFIX=.exe

OBJECTS:=$(patsubst ${SRC}/%.cpp, ${OBJ}/%.o,$(wildcard ${SRC}/*.cpp))
OBJECTS+= $(patsubst ${SRC}/%.c, ${OBJ}/%.o,$(wildcard ${SRC}/*.c))
TARGET=${BIN}/a${EXE_SUFFIX}
SUBDIRS=${OBJ} ${BIN}


all: ${SUBDIRS} ${TARGET}

${SUBDIRS}:%:
	@if [ ! -d $@ ]; then mkdir -p $@; fi;

${TARGET}: ${OBJECTS}
	${CXX} $^ -o $@ ${LINK_FLAGS}
	@echo "build $@ successfully"

sinclude ${OBJECTS:.o=.d}

$(OBJ)/%.o:${SRC}/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJ)/%.o:${SRC}/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ)/%.d:${SRC}/%.cpp
	@if [ ! -d ${OBJ} ]; then mkdir -p ${OBJ}; fi;
	@echo "Making $@ ..."
	@${CXX}  $< ${CXXFLAGS} -MM -MT"$(OBJ)/$*.o" -MT"$@" -MF"$@"

$(OBJ)/%.d:${SRC}/%.c
	@if [ ! -d ${OBJ} ]; then mkdir -p ${OBJ}; fi;
	@echo "Making $@ ..."
	@${CC}  $< ${CFLAGS} -MM -MT"$(OBJ)/$*.o" -MT"$@" -MF"$@"

.PRONY:clean cleanall all run
clean:
	@echo "Removing binary files......"
	-rm -rf $(OBJ)/*.o $(BIN)/*.*

cleanall:
	@echo "Removing all produced files......"
	-rm -rf $(OBJ) $(BIN)

run:all
	${TARGET}
