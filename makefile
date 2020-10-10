# Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
# Distributed under the terms of the GNU General Public License, version 3

TRANSMITTER=./transmitter
RECEIVER   =./receiver
SDIR	   =./src
IDIR	   =./include
ODIR   	   =./obj

CC		=gcc

IFLAGS	=-I$(IDIR)
CFLAGS_PARANOID =-pedantic -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wextra -Wfloat-equal -Wformat-nonliteral \
				 -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wno-unused-parameter \
				 -Wpointer-arith -Wshadow -Wstrict-prototypes -Wundef -Wunused-parameter -Wunused-result -Wwrite-strings

#CFLAGS=-Wall -g #-O3
CFLAGS=$(CFLAGS_PARANOID)

TRANSMITTER_O_FILES=$(ODIR)/transmitter.o
RECEIVER_O_FILES   =$(ODIR)/receiver.o

O_FILES=$(ODIR)/ll.o $(ODIR)/ll_internal.o $(ODIR)/ll_s_statemachine.o $(ODIR)/ll_u_statemachine.o $(ODIR)/ll_i_statemachine.o $(ODIR)/ll_utils.o $(ODIR)/application_layer.o

all: $(TRANSMITTER) $(RECEIVER)

$(TRANSMITTER): $(TRANSMITTER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(RECEIVER): $(RECEIVER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

test: FORCE
	make -C tests test

doc: FORCE
	cd doc && doxygen doxyfile

report.pdf: FORCE
	cd doc/report && latexmk --shell-escape report.tex -pdf
	cp doc/report/report.pdf .

clean: FORCE
	git clean -dfX

FORCE:
