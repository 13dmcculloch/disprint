#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <disprint/disprint.h>

int main(int argc, char **argv)
{
  struct disp_buf dims;

  memset(&dims, '0', sizeof dims);
  dims.delay = malloc(sizeof *(dims.delay));


  dims.hsize = 60;
  dims.vsize = 7;
  dims.delay->tv_sec = 0;
  dims.delay->tv_nsec = (unsigned long)(0.067 * 1e9);

  disflip("420:88", &dims);
  usleep(1000000);
  disflip("67:67:67", NULL);

  free(dims.delay);
  return disflip(NULL, NULL);
}
