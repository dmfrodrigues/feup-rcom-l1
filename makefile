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

EMITTER_O_FILES =$(ODIR)/writenoncanonical.o
RECEIVER_O_FILES=$(ODIR)/noncanonical.o

O_FILES=$(ODIR)/statemachine.o

all: $(EMITTER) $(RECEIVER)

$(EMITTER): $(EMITTER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(RECEIVER): $(RECEIVER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

FORCE:

report.pdf: FORCE
	cd doc/report && latexmk --shell-escape report.tex -pdf
	cp doc/report/report.pdf .

clean: FORCE
	git clean -dfX
