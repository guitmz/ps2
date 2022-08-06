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
#include <sifrpc.h>
#include <stdio.h>
#include <tamtypes.h>
#include <unistd.h>

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

  char buffer[] = { 'x' , 'y' , 'z' };
  int chars = fwrite(buffer, sizeof(char), sizeof(buffer), test);
  scr_printf("Number of items written to the file: %d\n", chars);

  fclose(test);

  SifExitRpc();
  return 0;
}
