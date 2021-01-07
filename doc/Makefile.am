BUILT_SOURCES = tecohlp.hlp tecomsg.messages tecomsg.errors
CLEANFILES = tecohlp.hlp tecomsg.messages tecomsg.errors

GENMSG = ../tools/genmsg
AM_V_GENMSG = $(am__v_GENMSG_$(V))
am__v_GENMSG_ = $(am__v_GENMSG_$(AM_DEFAULT_VERBOSITY))
am__v_GENMSG_0 = @echo "  GENMSG  " $@;
am__v_GENMSG_1 = 

GENHLP = cc -x c -E -P -w -traditional-cpp
AM_V_GENHLP = $(am__v_GENHLP_$(V))
am__v_GENHLP_ = $(am__v_GENHLP_$(AM_DEFAULT_VERBOSITY))
am__v_GENHLP_0 = @echo "  GENHLP  " $@;
am__v_GENHLP_1 = 

tecohlp.hlp: tecohlp.hlp_in tecomsg.messages tecomsg.errors
	$(AM_V_GENHLP)$(GENHLP) -otecohlp.hlp tecohlp.hlp_in

tecomsg.messages: ../src/tecomsg.src
	$(AM_V_GENMSG)$(GENMSG) -x -si -otecomsg.messages ../src/tecomsg.src

tecomsg.errors: ../src/tecomsg.src
	$(AM_V_GENMSG)$(GENMSG) -x -sw -otecomsg.errors ../src/tecomsg.src
