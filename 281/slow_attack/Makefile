CPP_CODE_FILES := $(sort $(wildcard *.cpp) $(wildcard *.cc))
CPP_OBJ_FILES := $(addsuffix .o,$(basename $(CPP_CODE_FILES)))
H_FILES := $(wildcard *.h)

.PHONY: all clean

all: main

main: $(CPP_OBJ_FILES) 
	g++ $(CPP_OBJ_FILES) -o main

%.o: %.cpp $(H_FILES)
	g++ $< -I -ldl -std=c++11 -c -o $@ -O3
	
%.o: %.cc $(H_FILES)
	g++ $< -I -ldl -std=c++11 -c -o $@ -O3

clean:
	-rm -rf *.o
	-rm main

submit: submit.tar.gz

submit.tar.gz: cipher.cpp
	tar -zcvf submit.tar.gz cipher.cpp
