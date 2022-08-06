# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = audio.elf
EE_OBJS = audio.o audioaudsrv_irx.o libsd_irx.o vox.o
EE_LIBS = -ldebug -lc -laudsrv

BIN2S = $(PS2SDK)/bin/bin2s

all: $(EE_BIN)

vox.s: vox.wav
	$(BIN2S) $< $@ vox

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
