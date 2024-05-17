cc=g++

.PHONY:all
all:udpClient udpServer

udpClient:udpClient.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread
udpServer:udpServer.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread

.PHONT:clean
clean:
	rm -r udpClient udpServer