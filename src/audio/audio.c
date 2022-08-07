/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# audsrv sample using callbacks
*/

#include <debug.h>
#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <tamtypes.h>

#include <audsrv.h>

extern u8 audsrv_irx[];
extern int size_audsrv_irx;
extern u8 libsd_irx[];
extern int size_libsd_irx;

extern u8 vox[];
extern int size_vox;

static int fillbuffer(void *arg) {
  iSignalSema((int)arg);
  return 0;
}

int main(int argc, char *argv[]) {
  int ret;
  int played;
  int err;
  int bytes;
  char chunk[2048];
  FILE *wav;
  ee_sema_t sema;
  int fillbuffer_sema;
  struct audsrv_fmt_t format;

  SifInitRpc(0);
  init_scr();
  //sleep(1);

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
  if (ret != 0) {
    scr_printf("sample: failed to initialize audsrv\n");
    scr_printf("audsrv returned error string: %s\n", audsrv_get_error_string());
    return 1;
  }

  format.bits = 8;
  format.freq = 11050;
  format.channels = 2;
  err = audsrv_set_format(&format);
  scr_printf("set format returned %d\n", err);
  scr_printf("audsrv returned error string: %s\n", audsrv_get_error_string());

  audsrv_set_volume(MAX_VOLUME);

  sema.init_count = 0;
  sema.max_count = 1;
  sema.option = 0;
  fillbuffer_sema = CreateSema(&sema);

  err = audsrv_on_fillbuf(sizeof(chunk), fillbuffer, (void *)fillbuffer_sema);
  if (err != AUDSRV_ERR_NOERROR) {
    scr_printf("audsrv_on_fillbuf failed with err=%d\n", err);
    goto loser;
  }

  wav = fopen("mc0:sound.wav", "rb");
  if (wav == NULL) {
    scr_printf("failed to open wav file\n");
    audsrv_quit();
    return 1;
  }

  fseek(wav, 0x30, SEEK_SET);

  scr_printf("starting play loop\n");
  played = 0;
  bytes = 0;
  while (1) {
    ret = fread(chunk, 1, sizeof(chunk), wav);
    if (ret > 0) {
      WaitSema(fillbuffer_sema);
      audsrv_play_audio(chunk, ret);
    }

    if (ret < sizeof(chunk)) {
      /* no more data */
      break;
    }

    played++;
    bytes = bytes + ret;

    if (played % 8 == 0) {
      scr_printf("\r%d bytes sent..\n", bytes);
    }

    if (played == 512)
      break;
  }

  fclose(wav);

loser:
  scr_printf("sample: stopping audsrv\n");
  audsrv_quit();

  scr_printf("sample: ended\n");
  return 0;
}
