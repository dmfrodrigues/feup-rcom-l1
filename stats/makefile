# Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
# Distributed under the terms of the GNU General Public License, version 3

CC		=gcc

all: stats

stats: recompile stats_transmitter stats_receiver

recompile: FORCE
	make -C .. clean
	make -C .. STATISTICS=1 STATISTICS_DELAY=1

stats_transmitter: stats_transmitter.c
	$(CC) $^ -o $@

stats_receiver: stats_receiver.c
	$(CC) $^ -o $@

test: recompile stats_transmitter stats_receiver
	./stats_receiver stats.txt 12 &
	sleep 1
	./stats_transmitter stats.txt 11
	./stats_receiver rate-size.txt 12 &
	sleep 1
	./stats_transmitter rate-size.txt 11
	./stats_receiver vary-tau.txt 12 &
	sleep 1
	./stats_transmitter vary-tau.txt 11

clean: FORCE
	rm -f stats_receiver stats_transmitter
	rm -f *.gif *.jpg *.png

FORCE:
