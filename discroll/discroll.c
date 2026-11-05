#include <stdio.h>

#include <disprint/disprint.h>

int main(int argc, char **argv)
{
  char buf[100];
  (void)fgets(buf, sizeof buf, stdin);
  dishscroll(buf, NULL);
  dishscroll(NULL, NULL);
  return 0;
}
