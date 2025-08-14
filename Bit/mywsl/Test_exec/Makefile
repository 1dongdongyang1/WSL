
myexec:myexec.c
	gcc -o $@ $^

.PHONY:all
all:myexec mybin

myexec:myexec.c
	gcc -o $@ $^
mybin:mybin.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -rf mybin myexec
