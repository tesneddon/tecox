include_HEADERS = tecodef.h tecomsg.h 
CLEANFILES = tecomsg.h

GENMSG = ../tools/genmsg
AM_V_GENMSG = $(am__v_GENMSG_$(V))
am__v_GENMSG_ = $(am__v_GENMSG_$(AM_DEFAULT_VERBOSITY))
am__v_GENMSG_0 = @echo "  GENMSG  " $@;
am__v_GENMSG_1 = 

tecomsg.h : ../src/tecomsg.src
	$(AM_V_GENMSG)$(GENMSG) -c -otecomsg.h ../src/tecomsg.src
