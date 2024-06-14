cc=g++
LD=#-DMYSELF

.PHONY:all
all:calClient calServer

calClient:calClient.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp ${LD}

calServer:calServer.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp ${LD}

.PHONT:clean
clean:
	rm -r calClient calServer