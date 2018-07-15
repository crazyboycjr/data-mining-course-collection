PACKAGE = main

PDF = ${PACKAGE}.pdf

all:  ${PDF}

%.pdf:  %.tex
	xelatex $<
	- bibtex $*
	xelatex $<
	xelatex $<
	while ( grep -q '^LaTeX Warning: Label(s) may have changed' $*.log) \
	do xelatex $<; done

clean:
	$(RM)  $(PACKAGE).cls *.log *.aux \
	*.cfg *.glo *.idx *.toc \
	*.ilg *.ind *.out *.lof \
	*.lot *.bbl *.blg *.gls *.cut *.hd \
	*.dvi *.ps *.thm *.tgz *.zip *.rpi

distclean: clean
	$(RM) $(PDF)

#
# Archive for the distribution. Includes typeset documentation
#
archive:  all clean
	tar -C .. -czvf ../$(PACKAGE).tgz --exclude '*~' --exclude '*.tgz' --exclude '*.zip'  --exclude CVS --exclude '.git*' $(PACKAGE); mv ../$(PACKAGE).tgz .

zip:  all clean
	zip -r  $(PACKAGE).zip * -x '*~' -x '*.tgz' -x '*.zip' -x CVS -x 'CVS/*'
