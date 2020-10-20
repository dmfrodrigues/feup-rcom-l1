# Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
# Distributed under the terms of the GNU General Public License, version 3

TRANSMITTER=../transmitter
RECEIVER   =../receiver

TRANSMITTER_PORT=11
RECEIVER_PORT   =12
TRANSMITTER_FILE=../testfiles/pinguim.gif
RECEIVER_FILE   =pinguim.gif

CALL_TRANSMITTER=$(TRANSMITTER) $(TRANSMITTER_PORT) $(TRANSMITTER_FILE)
CALL_RECEIVER   =$(RECEIVER) $(RECEIVER_PORT)

all:

test: clean test1 test2 test3 test4 test5 test6 test7

test1: $(TRANSMITTER)
	@echo ==================== $@ ====================
	$(CALL_TRANSMITTER) || echo "It should fail"
	sleep 1

test2: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_RECEIVER) &
	sleep 1
	$(CALL_TRANSMITTER)
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)

test3: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_TRANSMITTER) &
	sleep 0.5
	$(CALL_RECEIVER)
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)
	
test4: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_TRANSMITTER) &
	sleep 1.5
	$(CALL_RECEIVER)
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)

test5: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_RECEIVER) -s 1024 &
	sleep 1
	$(CALL_TRANSMITTER)  -s 1024
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)

test6: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_RECEIVER) -s 256 &
	sleep 1
	$(CALL_TRANSMITTER) -s 256
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)

test7: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	@rm -f $(RECEIVER_FILE)
	$(CALL_RECEIVER) -b 115200 &
	sleep 1
	$(CALL_TRANSMITTER) -b 115200
	sleep 2
	cmp $(TRANSMITTER_FILE) $(RECEIVER_FILE)
	@rm -f $(RECEIVER_FILE)

clean:
	make -C .. clean

$(TRANSMITTER): FORCE
	make -C ..

$(RECEIVER): FORCE
	make -C ..

FORCE:
