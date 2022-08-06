/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
*/

#include <debug.h>
#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include <tamtypes.h>
#include <unistd.h>

#include <audsrv.h>
#include <libsd.h>
/* #include <vox.h> */

extern u8 audsrv_irx[];
extern int size_audsrv_irx;
extern u8 libsd_irx[];
extern int size_libsd_irx;

extern u8 vox[];
extern int size_vox;

int main(int argc, char *argv[]) {
  int ret;
  int played;
  int err;
  char chunk[4096];
  FILE *wav;
  struct audsrv_fmt_t format;

  SifInitRpc(0);
  init_scr();
  sleep(1);

  // TMZ: write needed files to memory card
  scr_printf("COPYING FILES...\n");

  FILE *tmp;

  tmp = fopen("mc0:sound.wav", "w+");
  /* fwrite(shellcode, sizeof(shellcode), 1, tmp); */
  fwrite(vox, size_vox, 1, tmp);
  fclose(tmp);
  // END TMZ

  scr_printf("sample: kicking IRXs\n");
  SifExecModuleBuffer(libsd_irx, size_libsd_irx, 0, NULL, &ret);
  scr_printf("libsd loadmodule %d\n", ret);

  scr_printf("sample: loading audsrv\n");
  SifExecModuleBuffer(audsrv_irx, size_audsrv_irx, 0, NULL, &ret);
  scr_printf("audsrv loadmodule %d\n", ret);

  ret = audsrv_init();
  scr_printf("AUDIO INIT: %d\n", ret);
  if (ret != 0) {
    scr_printf("sample: failed to initialize audsrv\n");
    scr_printf("audsrv returned error string: %s\n", audsrv_get_error_string());
    return 1;
  }

  format.bits = 16;
  /* format.freq = 22050; */
  /* format.freq = 44100; */
  format.freq = 11000;
  format.channels = 1;
  err = audsrv_set_format(&format);
  scr_printf("set format returned %d\n", err);
  scr_printf("audsrv returned error string: %s\n", audsrv_get_error_string());

  audsrv_set_volume(MAX_VOLUME);

  wav = fopen("mc0:sound.wav", "rb");
  if (wav == NULL) {
    scr_printf("failed to open wav file\n");
    audsrv_quit();
    return 1;
  }

  ret = fseek(wav, 0x30, SEEK_SET);
  scr_printf("SEEK RET: %d\n", ret);

  scr_printf("starting play loop\n");
  played = 0;
  while (1) {
    ret = fread(chunk, 1, sizeof(chunk), wav);
    if (ret > 0) {
      audsrv_wait_audio(ret);
      audsrv_play_audio(chunk, ret);
    }

    if (ret < sizeof(chunk)) {
      /* no more data */
      break;
    }

    played++;
    if (played % 8 == 0) {
      scr_printf(".\n");
    }

    if (played == 512)
      break;
  }

  fclose(wav);

  scr_printf("sample: stopping audsrv\n");
  audsrv_quit();

  scr_printf("sample: ended\n");

  SifExitRpc();
  return 0;
}
