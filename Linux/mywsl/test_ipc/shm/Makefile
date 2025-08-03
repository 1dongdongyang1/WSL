.PHONY:all
all:shm_client shm_server

shm_client:shm_client.cc
	g++ -o $@ $^
shm_server:shm_server.cc
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f shm_client shm_server