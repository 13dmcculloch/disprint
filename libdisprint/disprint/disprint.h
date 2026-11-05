#define VSIZE 7  /* size of font */
#define HSIZE 60

#include <time.h>

struct disp_buf {
  char **buf;
  char *content;  /* static ascii content */
  size_t vsize;
  size_t hsize;
  size_t virt_hsize;
  size_t virt_vsize;
  size_t hidx;
  size_t vidx;
  struct timespec *delay;
};

/* to print once, with nullable disp_buf argument to amend size */ 
/* nullable s to free memory */
int disputs(const char *s, const struct disp_buf *dims);

/* scroll once horizontally over the input string s */
int dishscroll(const char *s, const struct disp_buf *dims);

/* previous characters are replaced with a flipping animation */
int disflip(const char *s, const struct disp_buf *dims);
