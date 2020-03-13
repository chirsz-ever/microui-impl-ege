CXX=g++
CC=gcc
CXXFLAGS=-std=c++11 -I.. -Wall 
CFLAGS=-std=c99 -I.. -Wall 
LINK_FLAGS= -L. -lgraphics64 -lkernel32 -luser32 -lgdi32 -lole32 -loleaut32 -limm32 -lmsimg32 -luuid -mwindows
EXE_SUFFIX=.exe

SOURCES = main.cpp microui_demo.c 
SOURCES += ../microui.c ../microui_impl_ege.cpp
SOURCES += ../atlas.c

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
TARGET = demo${EXE_SUFFIX}

all: ${TARGET}

${TARGET}: ${OBJS}
	${CXX} $^ -o $@ ${LINK_FLAGS}
	@echo "build $@ successfully"

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o:../%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o:../%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## run `make depends` to generate following dependency

main.o: main.cpp ../microui_impl_ege.h ../microui.h ../microui.h \
 microui_demo.h
microui_impl_ege.o: ../microui_impl_ege.cpp ../microui_impl_ege.h \
 ../microui.h ../atlas.h
microui_demo.o: microui_demo.c ../microui.h microui_demo.h
atlas.o: ../atlas.c ../microui.h ../atlas.h
microui.o: ../microui.c ../microui.h

.PRONY:clean all run depends
clean:
	-rm -rf *.o $(TARGET)

depends:
	@$(CXX) -MM $(CXXFLAGS) *.cpp ../*.cpp
	@$(CC) -MM $(CFLAGS) *.c ../*.c

run:all
	./${TARGET}
