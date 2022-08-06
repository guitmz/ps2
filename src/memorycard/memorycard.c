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
#include <tamtypes.h>
//#include <fileio.h>
#include <libmc.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  SifInitRpc(0);
  init_scr();

  sleep(2);

  scr_printf("Hello, World!\n");

  // After 5 seconds, clear the screen.
  sleep(5);
  scr_clear();

  // Move cursor to 20, 20
  scr_setXY(20, 20);
  scr_printf("Hello Again, World!\n");

  sleep(10);

  FILE *test;
  test = fopen("mc0:test.dat", "wb+");

  if (test == NULL) {
    scr_printf("Error opening file\n");
    return 1;
  }

  char buffer[] = {'x', 'y', 'z'};
  int chars = fwrite(buffer, sizeof(char), sizeof(buffer), test);
  scr_printf("Number of items written to the file: %d\n", chars);

  fclose(test);
  scr_printf("Test file closed\n");

  int sv = CreateSave();
  scr_printf("Saved created, result %d\n", sv);

  SifExitRpc();
  return 0;
}

int CreateSave(void) {
  int mc_fd;
  int icon_fd, icon_size;
  char *icon_buffer;
  mcIcon icon_sys;

  static iconIVECTOR bgcolor[4] = {
      {68, 23, 116, 0},   // top left
      {255, 255, 255, 0}, // top right
      {255, 255, 255, 0}, // bottom left
      {68, 23, 116, 0},   // bottom right
  };

  static iconFVECTOR lightdir[3] = {
      {0.5, 0.5, 0.5, 0.0},
      {0.0, -0.4, -0.1, 0.0},
      {-0.5, -0.5, 0.5, 0.0},
  };

  static iconFVECTOR lightcol[3] = {
      {0.3, 0.3, 0.3, 0.00},
      {0.4, 0.4, 0.4, 0.00},
      {0.5, 0.5, 0.5, 0.00},
  };

  static iconFVECTOR ambient = {0.50, 0.50, 0.50, 0.00};

  if (mkdir("mc0:PS2DEV", 0777) < 0)
    return -1;

  // Set up icon.sys. This is the file which controls how our memory card save
  // looks in the PS2 browser screen. It contains info on the bg colour,
  // lighting, save name and icon filenames. Please note that the save name is
  // sjis encoded.

  memset(&icon_sys, 0, sizeof(mcIcon));
  strcpy(icon_sys.head, "PS2D");
  strcpy_sjis((short *)&icon_sys.title, "Memcard Example\nPS2Dev r0x0rs");
  icon_sys.nlOffset = 16;
  icon_sys.trans = 0x60;
  memcpy(icon_sys.bgCol, bgcolor, sizeof(bgcolor));
  memcpy(icon_sys.lightDir, lightdir, sizeof(lightdir));
  memcpy(icon_sys.lightCol, lightcol, sizeof(lightcol));
  memcpy(icon_sys.lightAmbient, ambient, sizeof(ambient));
  strcpy(icon_sys.view,
         "ps2dev.icn"); // these filenames are relative to the directory
  strcpy(icon_sys.copy, "ps2dev.icn"); // in which icon.sys resides.
  strcpy(icon_sys.del, "ps2dev.icn");

  // Write icon.sys to the memory card (Note that this filename is fixed)
  mc_fd = open("mc0:PS2DEV/icon.sys", "w+");
  if (mc_fd < 0)
    return -2;

  write(mc_fd, &icon_sys, sizeof(icon_sys));
  close(mc_fd);
  scr_printf("icon.sys written sucessfully.\n");

  // Write icon file to the memory card.
  // Note: The icon file was created with my bmp2icon tool, available for
  // download at
  //       http://www.ps2dev.org
  icon_fd = open("host:ps2dev.icn", "r");
  if (icon_fd < 0)
    return -3;

  icon_size = lseek(icon_fd, 0, 2);
  lseek(icon_fd, 0, SEEK_SET);

  icon_buffer = malloc(icon_size);
  if (icon_buffer == NULL)
    return -4;
  if (read(icon_fd, icon_buffer, icon_size) != icon_size)
    return -5;
  close(icon_fd);

  icon_fd = open("mc0:PS2DEV/ps2dev.icn", "w+");
  if (icon_fd < 0)
    return -6;

  write(icon_fd, icon_buffer, icon_size);
  close(icon_fd);
  scr_printf("ps2dev.icn written sucessfully.\n");

  scr_printf("ALL DONE!");
  return 0;
}
