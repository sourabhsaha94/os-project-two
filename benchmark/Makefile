all: benchmark validate test test1 get delete semaphore

benchmark: benchmark.c
	$(CC) -g -O0 benchmark.c -o benchmark -lckv
validate: validate.c
	$(CC) -g -O0 validate.c -o validate -lckv
test: test.c
	$(CC) -g -O0 test.c -o test -lckv
test1: test1.c
	$(CC) -g -O0 test1.c -o test1 -lckv

get: get.c
	$(CC) -g -O0 get.c -o get -lckv

delete: delete.c
	$(CC) -g -O0 delete.c -o delete -lckv

semaphore: semaphore.c
	$(CC) -g -O0 semaphore.c -o semaphore -lckv -lpthread


clean:
	rm -f benchmark validate test test1 get delete semaphore
