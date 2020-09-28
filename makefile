all:

FORCE:

report.pdf: FORCE
	cd doc/report && latexmk --shell-escape report.tex -pdf
	cp doc/report/report.pdf .
