# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = adpcm.elf
EE_OBJS = adpcm.o audsrv_irx.o libsd_irx.o laugh.o
EE_LIBS = -ldebug -lc -laudsrv

BIN2S = $(PS2SDK)/bin/bin2s

all: $(EE_BIN)

%.adp : %.wav
	$(PS2SDK)/bin/adpenc $< $@

laugh.s: laugh.adp
	$(BIN2S) $< $@ laugh

audsrv_irx.s: $(PS2SDK)/iop/irx/audsrv.irx
	$(BIN2S) $< $@ audsrv_irx

libsd_irx.s: $(PS2SDK)/iop/irx/libsd.irx
	$(BIN2S) $< $@ libsd_irx

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
