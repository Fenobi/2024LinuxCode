cc=g++

.PHONY:all
all:udpClient_linux udpServer

udpClient:udpClient_linux.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread
udpServer:udpServer.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread

.PHONT:clean
clean:
	rm -r udpClient_linux udpServer