CC = gcc
CFLAGS = -Wall -Werror -std=c99 -m64

csim: csim.c parse-address.c 
	$(CC) $(CFLAGS) -o cache_sim csim.c
test: 
	$(CC) $(CFLAGS) -o testCacheSim test-csim.c
clean:
	rm .cache_output
