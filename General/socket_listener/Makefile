CPP_CODE_FILES := $(wildcard *.cpp)
CPP_OBJ_FILES := $(addsuffix .o,$(basename $(CPP_CODE_FILES)))
H_FILES := $(wildcard *.h)

.PHONY: all
.SUFFIXES:

all: main

main: $(CPP_OBJ_FILES)
	g++ $< -std=c++11 -o $@ -pthread

%.o: %.cpp $(H_FILES)
	@echo "Building $@"
	g++ $< -std=c++11 -c -o $@ -g

clean:
	rm -f *.o
	rm -f main
	rm out.*
