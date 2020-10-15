CC		=gcc

all:

stats: recompile stats_transmitter stats_receiver

recompile: FORCE
	make -C .. clean
	make -C .. STATISTICS=1

stats_transmitter: stats_transmitter.c
	$(CC) $^ -o $@

stats_receiver: stats_receiver.c
	$(CC) $^ -o $@

FORCE:
