stubobjfiles= \
uti-stub.o  \
smtrace.o  

stublib=libsmtrace.so

.PHONY:all
all:test $(stublib) testMtrace

test:test.c
	gcc -ggdb $^ -o $@

testMtrace:testMtrace.c
	gcc -ggdb $^ -o $@

%.o:%.c
	gcc -ggdb -fPIC -c $^ -o $@

$(stublib):$(stubobjfiles)
	ld -share -o $@ -lpthread $(stubobjfiles)

.PHONY:clean
clean:
	rm -rf *.o test *.so testMtrace

