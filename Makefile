executable:=hotpotato
library:=libtest

tmp:=./tmp
src:=./src
test:=./test

#objects:=$(tmp)/entrada.o

#sources:=$(src)/entrada.cc

#cxxflags:= -g -std=c++11 -Wall
cxxflags:= -g -std=c++0x -Wall -O3
cxx:=mpic++
#thread:=-lpthread

includes:=-I./ -I./include -I../api/include
libs:=-L./ -L./lib

main: $(objects)
	$(cxx) $(includes) $(libs) -o $(executable) $(executable).cc $(cxxflags)


clean:
	rm -rf $(tmp);
	rm -f $(executable);