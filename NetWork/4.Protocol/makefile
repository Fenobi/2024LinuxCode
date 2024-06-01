cc=g++

.PHONY:all
all:calClient calServer

calClient:calClient.cc
	$(cc) -o $@ $^ -std=c++11 #-lpthread
calServer:calServer.cc
	$(cc) -o $@ $^ -std=c++11 #-lpthread

.PHONT:clean
clean:
	rm -r calClient calServer