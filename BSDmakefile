CFLAGS+=-DTECO_K_WORD_SIZE=32 -g -DTECO_K_VERSION=41
LDFLAGS+=-static

.PHONY : all clean rebuild

all : teco

clean :
	rm -rRf *.o *.cat teco

rebuild : clean all

teco : crtrub.o main.o teco.o tecoio.o tecomsg.o tecounix.o
	cc -o $@ $(.ALLSRC)

teco.pdf : teco.xml
	xmllint --xinclude $^ | xmlstarlet val --net --err --xsd /usr/share/xml/docbook/schema/xsd/5.0/docbook.xsd
	xsltproc /usr/share/xml/docbook/stylesheet/docbook-xsl-ns/fo/docbook.xsl book.xml > book.fo
	fop -fo book.fo -pdf book.pdf

teco.xml : ch_bas.xml intro.xml

