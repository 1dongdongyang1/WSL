cc=g++

.PHONY:all
all:udpClient udpServer

udpClient:udpClient.cc
	$(cc) -o $@ $^
udpServer:udpServer.cc
	$(cc) -o $@ $^

.PHONY:clean
clean:
	rm -f udpClient udpServer

