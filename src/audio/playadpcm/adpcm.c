/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# audsrv adpcm sample
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

extern u8 laugh[];
extern int size_laugh;

int main(int argc, char *argv[]) {
  /* Uncomment to hear two samples played simultaenously
  int i;
  */
  int ret;
  FILE *adpcm;
  audsrv_adpcm_t sample;
  int size;
  u8 *buffer;

  SifInitRpc(0);
  init_scr();

  // TMZ: write needed files to memory card
  scr_printf("COPYING FILES...\n");

  FILE *tmp;

  tmp = fopen("mc0:laugh.adp", "w+");
  /* fwrite(shellcode, sizeof(shellcode), 1, tmp); */
  fwrite(laugh, size_laugh, 1, tmp);
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

  adpcm = fopen("mc0:laugh.adp", "rb");

  if (adpcm == NULL) {
    scr_printf("failed to open adpcm file\n");
    audsrv_quit();
    return 1;
  }

  fseek(adpcm, 0, SEEK_END);
  size = ftell(adpcm);
  fseek(adpcm, 0, SEEK_SET);

  buffer = malloc(size);

  fread(buffer, 1, size, adpcm);
  fclose(adpcm);

  scr_printf("playing sample..\n");

  audsrv_adpcm_init();
  audsrv_set_volume(MAX_VOLUME);
  audsrv_adpcm_set_volume(0, MAX_VOLUME);
  audsrv_load_adpcm(&sample, buffer, size);
  audsrv_ch_play_adpcm(0, &sample);

  /* Uncomment to hear two samples played simultaenously
  for (i=0; i<100; i++)
  {
          nopdelay();
  }

  audsrv_adpcm_set_volume(1, MAX_VOLUME);
  audsrv_ch_play_adpcm(1, &sample);
  */

  scr_printf("sample played..\n");

  free(buffer);

  while (1)
    ;
  return 0;
}
