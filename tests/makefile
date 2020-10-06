TRANSMITTER=../transmitter
RECEIVER   =../receiver

TRANSMITTER_PORT=11
RECEIVER_PORT   =12

CALL_TRANSMITTER=$(TRANSMITTER) $(TRANSMITTER_PORT)
CALL_RECEIVER   =$(RECEIVER) $(RECEIVER_PORT)

all:

test: clean test1 test2 test3 test4

test1: $(TRANSMITTER)
	@echo ==================== $@ ====================
	$(CALL_TRANSMITTER)
	sleep 1

test2: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	$(CALL_RECEIVER) &
	sleep 1
	$(CALL_TRANSMITTER)
	sleep 4

test3: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	$(CALL_TRANSMITTER) &
	sleep 0.5
	$(CALL_RECEIVER)
	sleep 4
	
test4: $(TRANSMITTER) $(RECEIVER)
	@echo ==================== $@ ====================
	$(CALL_TRANSMITTER) &
	sleep 1.5
	$(CALL_RECEIVER)
	sleep 4

clean:
	make -C .. clean

$(TRANSMITTER):
	make -C ..

$(RECEIVER):
	make -C ..
