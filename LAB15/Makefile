CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic

all: receiver sender block_sigint event_loop

receiver: receiver.c
	$(CC) $(CFLAGS) $< -o $@

sender: sender.c
	$(CC) $(CFLAGS) $< -o $@

block_sigint: block_sigint.c
	$(CC) $(CFLAGS) $< -o $@

event_loop: event_loop.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f receiver sender block_sigint event_loop

.PHONY: all clean