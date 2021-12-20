.SUFFIXES: .eps .png .fig .svg .pdf .plt .pltline
.PHONY: pdf all clean test test2 print spellcheck showboundingbox

FILENAME = Vortrag
DRUCKER = XeroxD

GRAPHICS_PLT = $(wildcard figs/*.plt)
GRAPHICS_SVG = $(wildcard figs/*.svg)
GRAPHICS_PLTLINE = $(wildcard figs/*.pltline)
GRAPHICS = $(GRAPHICS_SVG:.svg=.eps) $(GRAPHICS_PLT:.plt=.eps) $(GRAPHICS_PLTLINE:.pltline=.eps)
GRAPHICS_PDF = $(GRAPHICS:.eps=.pdf)

#INCLUDES = Table_Trampoline_64_1.tex Table_Trampoline_64_2.tex
INCLUDES_AUX = $(INCLUDES:.tex=.aux)

LISTINGS = $(wildcard listings/*.s listings/*.c)

all: $(FILENAME).dvi

clean:
	rm -f $(FILENAME).dvi $(FILENAME).ps $(FILENAME).pdf
	rm -f $(FILENAME).aux $(FILENAME).toc $(FILENAME).bbl $(FILENAME).blg
	rm -f $(FILENAME).log $(FILENAME).out *.bak
	rm -f $(FILENAME).snm $(FILENAME).nav
	rm -f $(GRAPHICS)
	rm -f $(GRAPHICS_PDF)
	rm -f $(INCLUDES_AUX)

test: $(FILENAME).dvi
	if ! ps ax | grep "xdvi $(FILENAME).dvi$$"; then xdvi $(FILENAME).dvi & fi

test2: $(FILENAME).pdf
	evince $(FILENAME).pdf

pdf: $(FILENAME).pdf

print: $(FILENAME).ps
	lpr -P$(DRUCKER) $(FILENAME).ps

spellcheck:
	aspell -l en -c $(FILENAME).tex

$(FILENAME).dvi: $(FILENAME).tex $(GRAPHICS) $(INCLUDES) $(LISTINGS)
	-latex $(FILENAME).tex | grep -E '^LaTeX Warning'

$(FILENAME).ps: $(FILENAME).pdf
	pdftops $(FILENAME).pdf

$(FILENAME).pdf: $(FILENAME).tex $(GRAPHICS_PDF) $(INCLUDES) $(LISTINGS)
	-pdflatex $(FILENAME).tex | grep -E '^LaTeX Warning'
	-pdflatex $(FILENAME).tex | grep -E '^LaTeX Warning'

showboundingbox: $(GRAPHICS)
	grep '^%%BoundingBox' figs/*.eps | awk '{printf("%-51s %4d %4d %4d %4d (%dx%d)\n", substr($$1, 0, length($$1) - 14), $$2, $$3, $$4, $$5, $$4 - $$2, $$5 - $$3);}'

.png.eps:
	convert $< $*.eps

.plt.eps:
	pl -o $*.eps -eps $<

.pltline.eps:
	pl -prefab lines data=$< x=1 y=2 pointsym=none -o $*.eps -eps

.svg.eps:
	inkscape --without-gui --export-eps=$*.eps $<

.fig.eps:
	fig2dev -L eps $< $*.eps

.eps.pdf:
	epstopdf $<

