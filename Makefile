include ../../Makefile.defs
auto_gen=
NAME=rtc_gw.so

DEFS+=-I$(LOCALBASE)/lib

ifneq ($(CURL_BUILDER),)
	CURLDEFS += $(shell $(CURL_BUILDER) --cflags)
	CURLLIBS += $(shell $(CURL_BUILDER) --libs)
else
	CURLDEFS+=-I$(LOCALBASE)/include -I$(SYSBASE)/include
	CURLLIBS+=-L$(LOCALBASE)/lib -L$(SYSBASE)/lib -lcurl -levent
endif

DEFS+=$(CURLDEFS)
LIBS=$(CURLLIBS)

DEFS+=-DKAMAILIO_MOD_INTERFACE
include ../../Makefile.modules
