.PHONY:all
all:mysignal mytest

mysignal:mysignal.cc
	g++ -o $@ $^ 
mytest:mytest.cc
	g++ -o $@ $^ 

.PHONY:clean
clean:
	rm mysignal mytest