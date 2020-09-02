CXX        = g++
CC         = gcc
ROOT       = ../..
EGE_INCLUDE=
EGE_LIB    = -lgraphics64
CXXFLAGS   = -std=c++11 -I$(ROOT) -Wall -g $(EGE_INCLUDE)
CFLAGS     = -std=c99 -I$(ROOT) -Wall -g
LINK_FLAGS = $(EGE_LIB) -lkernel32 -luser32 -lgdi32 -lole32 -loleaut32\
             -limm32 -lmsimg32 -luuid -mwindows -lgdiplus
EXE_SUFFIX = .exe

SOURCES =  main.cpp
SOURCES += $(ROOT)/microui.c $(ROOT)/microui_impl_ege.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
TARGET = calculator${EXE_SUFFIX}

all: ${TARGET}

${TARGET}: ${OBJS}
	${CXX} $^ -o $@ ${LINK_FLAGS}
	@echo "build $@ successfully"

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o:$(ROOT)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o:$(ROOT)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## run `make depends` to generate following dependency

main.o: main.cpp ../../microui_impl_ege.h ../../microui.h ../../microui.h
microui_impl_ege.o: ../../microui_impl_ege.cpp ../../microui_impl_ege.h \
 ../../microui.h
microui.o: ../../microui.c ../../microui.h

## end dependency

.PRONY:clean all run depends
clean:
	-rm -rf *.o $(TARGET)

depends:
	@$(CXX) -MM $(CXXFLAGS) *.cpp $(ROOT)/*.cpp
	@$(CC) -MM $(CFLAGS) $(ROOT)/*.c

run:all
	./${TARGET}
