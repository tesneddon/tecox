AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib
BUILT_SOURCES = tecomsg.cat
CLEANFILES = tecomsg.cat
bin_PROGRAMS = tecox
tecox_SOURCES = main.c
tecox_LDADD = ../lib/libteco.a $(CURSES_LIB)

tecomsg.cat : tecomsg.msgx
	gencat --new --output=$@ $<
