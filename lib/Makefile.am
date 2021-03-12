BUILT_SOURCES = messages.c
CLEANFILES = messages.c

GENMSG = ../tools/genmsg
AM_V_GENMSG = $(am__v_GENMSG_$(V))
am__v_GENMSG_ = $(am__v_GENMSG_$(AM_DEFAULT_VERBOSITY))
am__v_GENMSG_0 = @echo "  GENMSG  " $@;
am__v_GENMSG_1 = 

AM_CPPFLAGS = -I$(top_srcdir)/include -O0
lib_LIBRARIES = libteco.a
libteco_a_SOURCES = tecomsg.c tecounix.c crtrub.c teco.c tecoio.c \
                    tecoutil.c \
                    messages.c

messages.c: ../src/tecomsg.src
	$(AM_V_GENMSG)$(GENMSG) -m -omessages.c ../src/tecomsg.src
