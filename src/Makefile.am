AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib
BUILT_SOURCES = tecomsg.cat tecomsg.msg
CLEANFILES = tecomsg.cat tecomsg.msg
bin_PROGRAMS = tecox
tecox_SOURCES = main.c
tecox_LDADD = ../lib/libteco.a $(CURSES_LIB)

tecomsg.msg : tecomsg.src

tecomsg.cat: tecomsg.msgx

tecomsg.msgx: tecomsg.src

#tecomsg.cat : tecomsg.msgx
#	gencat --new --output=$@ $<
