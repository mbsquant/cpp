CC = g++
CCFLAGS = -Wall -Ofast -Wno-strict-aliasing#-g
MATHFLAGS = -msse4.2 -ffast-math 
EXTRAFLAGS=-DUSE_APPR -DUSE_FASTRAND#-DNO_DEQHIST_UPDATE
#PROFILING=-pg
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
CC = g++-4.7
LKFLAGS = -lboost_thread
CCFLAGS += -std=c++11
endif
ifeq ($(UNAME), MINGW32_NT-6.1)
IDIR = -I C:/MinGW/include/
LKFLAGS =-lboost_thread -lboost_system -lboost_chrono
endif


CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(notdir $(CPP_FILES:.cpp=.o))

# Some programs include headers defined in earlier chapters
# LOCFLAGS used to set tell the compiler where to find a
# header that is not in the same directory as the source file itself
# LOCFLAGS will be set in directory level makefiles as needed

all:  LoanSoft.exe

LoanSoft.exe: $(OBJ_FILES)
	$(CC) -o $@ $^ $(LKFLAGS) $(PROFILING)

%.o: %.cpp
	$(CC) $(CCFLAGS) $(MATHFLAGS) $(EXTRAFLAGS) $(IDIR) -c $< -o $@

# rule that says how to make a .exe executable file from a .o object file


# target to clean up the object files and any core files
# executing "make clean" in a subdirectory will remove all
# files named core or any file ending in .obj, or .stackdump
clean:
	rm -rf *.o core *.stackdump

# target to remove executable files as well as object and core files
clobber: clean
	rm -rf *.exe 
