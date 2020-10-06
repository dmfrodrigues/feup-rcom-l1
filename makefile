EMITTER	=./emitter
RECEIVER=./receiver
SDIR	=./src
IDIR	=./include
ODIR   	=./obj

CC		=gcc

IFLAGS	=-I$(IDIR)
CFLAGS_PARANOID =-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wbad-function-cast \
-Wstrict-prototypes -Winline -Wundef \
-Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings -Wunused-parameter \
-Wfloat-equal -pedantic -Wno-long-long -Wno-endif-labels -Wunused-result -g 
CFLAGS 	=-Wall -g #-O3
#CFLAGS=$(CFLAGS_PARANOID)

EMITTER_O_FILES =$(ODIR)/emitter.o
RECEIVER_O_FILES=$(ODIR)/receiver.o

O_FILES=$(ODIR)/statemachine.o $(ODIR)/utils.o $(ODIR)/ll.o

all: $(EMITTER) $(RECEIVER)

$(EMITTER): $(EMITTER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(RECEIVER): $(RECEIVER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

test: FORCE
	make -C tests test

report.pdf: FORCE
	cd doc/report && latexmk --shell-escape report.tex -pdf
	cp doc/report/report.pdf .

clean: FORCE
	git clean -dfX

FORCE:
