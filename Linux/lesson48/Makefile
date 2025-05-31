cc=g++

.PHONY:all
all:calClient calServer

calClient:calClient.cc
	$(cc) -o $@ $^ -ljsoncpp 
calServer:calServer.cc
	$(cc) -o $@ $^ -ljsoncpp 

.PHONY:clean
clean:
	rm -f calClient calServer

