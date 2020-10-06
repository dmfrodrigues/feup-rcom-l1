EMITTER	=../emitter
RECEIVER=../receiver

EMITTER_PORT =/dev/ttyS10
RECEIVER_PORT=/dev/ttyS11

CALL_EMITTER =$(EMITTER) $(EMITTER_PORT)
CALL_RECEIVER=$(RECEIVER) $(RECEIVER_PORT)

all:

test: clean test1 test2 test3 test4

test1: $(EMITTER)
	echo ==================== $@ ====================
	$(CALL_EMITTER)

test2: $(EMITTER) $(RECEIVER)
	echo ==================== $@ ====================
	$(CALL_RECEIVER) &
	sleep 1
	$(CALL_EMITTER)

test3: $(EMITTER) $(RECEIVER)
	echo ==================== $@ ====================
	$(CALL_EMITTER) &
	sleep 0.5
	$(CALL_RECEIVER)
	
test4: $(EMITTER) $(RECEIVER)
	echo ==================== $@ ====================
	$(CALL_EMITTER) &
	sleep 1.5
	$(CALL_RECEIVER)

clean:
	make -C .. clean

$(EMITTER):
	make -C ..

$(RECEIVER):
	make -C ..
