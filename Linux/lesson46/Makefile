cc=g++

.PHONY:all
all:tcpClient tcpServer

tcpClient:tcpClient.cc
	$(cc) -o $@ $^
tcpServer:tcpServer.cc
	$(cc) -o $@ $^

.PHONY:clean
clean:
	rm -f tcpClient tcpServer

