cc=g++

.PHONY:all
all:tcpClient tcpServer

tcpClient:tcpClient.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread
tcpServer:tcpServer.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread

.PHONT:clean
clean:
	rm -r tcpClient tcpServer