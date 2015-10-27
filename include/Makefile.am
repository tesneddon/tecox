BUILT_SOURCES = tecomsg.h
CLEANFILES = tecomsg.h
include_HEADERS = tecodef.h tecomsg.h

tecomsg.h : ../src/tecomsg.msgx
	( echo "#ifndef tecomsg_h__" ; \
	  echo "#define tecomsg_h__" ; \
	  echo "#define TECO__NORMAL 0" ; \
	  gencat --new --output=/dev/null --header=- $<; \
	  echo "#endif /* tecomsg_h__ */" ) > $@
