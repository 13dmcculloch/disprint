#include <stdio.h>

#include <disprint/disprint.h>

int main(int argc, char **argv)
{
  char buf[100];
  (void)fgets(buf, sizeof buf, stdin);
  disputs(buf, NULL);
  disputs(NULL,NULL);
  return 0;
}
