#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

#include <prat.h>
#include <disprint/disprint.h>
#include <disprint/fonts.h>

#define VSIZE 7  /* size of font */
#define SYMBOL_HSIZE 7
#define SYMBOL_WIDTH SYMBOL_HSIZE
#define HSIZE 60
#define RELEASE_THRESHOLD 1024
#define DELAY_NS (unsigned long)(0.05 * 1e9)

static int _clean_up(struct disp_buf *b)
{
    if(CHKPTR(b))
        return -1;
    if(CHKPTR(b->buf))
        return -1;
    if(CHKPTR(*b->buf))
        return -1;

    free(*b->buf);  /* should free f_buf */
    *b->buf = NULL;
    free(b->buf);
    b->buf = NULL;
    free(b->content);
    b->content = NULL;
    free(b->delay);
    b->delay = NULL;
    free(b);
    b = NULL;

    return 0;
}

static void print_buf(const struct disp_buf *b)
{
  size_t row;

  for(row = b->vsize; row < b->virt_vsize; ++row) {
    fwrite(b->buf[row], 1, b->hsize, stdout);
    putc('\n', stdout);
  }
}

static void print_virt_buf(const struct disp_buf *b)
{
  size_t row;

  for(row = 0; row < b->virt_vsize; ++row) {
    fwrite(b->buf[row], 1, b->virt_hsize, stdout);
    putc('\n', stdout);
  }
}

static void clear_screen(const struct disp_buf *b)
{
  printf("\033[%dA\r", b->vsize);
}
  

/* insert a symbol at hidx,vidx, modifying b.hidx */
static int insert_symbol_at(struct disp_buf *b, const char **symbol,
    size_t hidx, size_t vidx)
{
  size_t symbol_width, symbol_height, col;
  
  symbol_width = strlen(symbol[0]);
  symbol_height = sizeof symbol - 1;

  if(b->vsize != symbol_height) {
    puts("Symbol height not equal to print buffer height.\n");
    return -1;
  }
  if(hidx > b->virt_hsize-symbol_width || 
    vidx > b->virt_vsize-symbol_height) {
    puts("Indices are outside of buffer dimensions.\n");
    return -1;
  }

  for(col = 0; col < symbol_width; ++col)
    strcpy(b->buf[vidx + col] + hidx, symbol[col]);
  b->hidx += symbol_width;

  return 0;
}

/* this function adds a new symbol to the buffer from the left */
static int push_symbol(struct disp_buf *b, const char **symbol)
{
  size_t symbol_width, symbol_height;

  symbol_width = strlen(symbol[0]);
  symbol_height = sizeof symbol - 1;

  if(b->hidx > b->virt_hsize-symbol_width) {
    puts("No space left in buffer for new symbol.\n");
    return -1;
  }

  return insert_symbol_at(b, symbol, b->hidx, symbol_height);
}

static const char **char_to_symbol(char c)
{
  const char **rc = NULL;

  switch (toupper((char) c)) {
    case 'A':
        rc = char_A;
        break;
    case 'B':
        rc = char_B;
        break;
    case 'C':
        rc = char_C;
        break;
    case 'D':
        rc = char_D;
        break;
    case 'E':
        rc = char_E;
        break;
    case 'F':
        rc = char_F;
        break;
    case 'G':
        rc = char_G;
        break;
    case 'H':
        rc = char_H;
        break;
    case 'I':
        rc = char_I;
        break;
    case 'J':
        rc = char_J;
        break;
    case 'K':
        rc = char_K;
        break;
    case 'L':
        rc = char_L;
        break;
    case 'M':
        rc = char_M;
        break;
    case 'N':
        rc = char_N;
        break;
    case 'O':
        rc = char_O;
        break;
    case 'P':
        rc = char_P;
        break;
    case 'Q':
        rc = char_Q;
        break;
    case 'R':
        rc = char_R;
        break;
    case 'S':
        rc = char_S;
        break;
    case 'T':
        rc = char_T;
        break;
    case 'U':
        rc = char_U;
        break;
    case 'V':
        rc = char_V;
        break;
    case 'W':
        rc = char_W;
        break;
    case 'X':
        rc = char_X;
        break;
    case 'Y':
        rc = char_Y;
        break;
    case 'Z':
        rc = char_Z;
        break;
    case '0':
        rc = char_0;
        break;
    case '1':
        rc = char_1;
        break;
    case '2':
        rc = char_2;
        break;
    case '3':
        rc = char_3;
        break;
    case '4':
        rc = char_4;
        break;
    case '5':
        rc = char_5;
        break;
    case '6':
        rc = char_6;
        break;
    case '7':
        rc = char_7;
        break;
    case '8':
        rc = char_8;
        break;
    case '9':
        rc = char_9;
        break;
    case ' ':
        rc = char_SP;
        break;
    case ':':
        rc = char_col;
        break;
    default:
      rc = char_SP;
      break;
  }

  return rc;
}

/* add character to buffer using push_symbol */
static int push_char(struct disp_buf *b, char c)
{
  return push_symbol(b, char_to_symbol(c));
}

static struct disp_buf *new_display_buffer(size_t hsize, size_t vsize,
  long delay_ns)
{
  struct disp_buf *db;
  char *f_buf, **buf;
  size_t virt_hsize, virt_vsize, row;
  struct timespec *delay;

  virt_hsize = hsize + SYMBOL_WIDTH;
  virt_vsize = vsize*2;

  if(delay_ns > 999999999) {
    puts("Delay too long.\n");
    return NULL;
  }

  db = malloc(sizeof *db);
  if(CHKPTR(db)) {
    perror("malloc");
    return NULL;
  }

  /* allocate framebuffer */
  f_buf = malloc(virt_vsize * virt_hsize * sizeof *f_buf);
  if(CHKPTR(f_buf)) {
    perror("malloc");
    free(db);
    return NULL;
  }

  /* row pointer allocation */
  buf = malloc(virt_vsize * sizeof *buf);
  if(CHKPTR(buf)) {
    perror("malloc");
    free(f_buf);
    free(db);
    return NULL;
  }
  /* row pointer initialisation */
  for(row = 0; row < virt_vsize; ++row) {
    buf[row] = f_buf + row * virt_hsize;
    memset(buf[row], ' ', virt_hsize-2);
    buf[row][virt_hsize-1] = '\0';
  }

  delay = malloc(sizeof *delay);
  if(CHKPTR(delay)) {
    perror("malloc");
    free(buf);
    free(f_buf);
    free(db);
    free(delay);
    return NULL;
  }

  delay->tv_sec = 0;
  delay->tv_nsec = delay_ns;

  db->buf = buf;
  db->vsize = vsize;
  db->hsize = hsize;
  db->virt_hsize = virt_hsize;
  db->virt_vsize = virt_vsize;
  db->hidx = 0;
  db->vidx = 0;
  db->content = NULL;
  db->delay = delay;

  return db;
}

static void clear_buf(struct disp_buf *db)
{
  size_t row;

  for(row = 0; row < db->vsize; ++row) {
    memset(db->buf[row], ' ', db->virt_hsize-2);
    db->buf[row][db->virt_hsize-1] = '\0';
  }
}

int disputs(const char *s, const struct disp_buf *dims)
{
  static struct disp_buf *db = NULL;
  static int f_bufexists = 0;
  size_t row, s_idx, col;

  s_idx = 0;
  col = 0;

  if(!s && f_bufexists) {
    f_bufexists = 0;
    return _clean_up(db);
  }

  if(!f_bufexists) {
    if(dims)
      db = new_display_buffer(dims->hsize, dims->vsize, 
        dims->delay->tv_nsec);
    else
      db = new_display_buffer(HSIZE, VSIZE, DELAY_NS);

    if(CHKPTR(db)) {
      puts("Could not create display buffer.\n");
      return 1;
    }

    f_bufexists = 1;
  }

  if(CHKPTR(s)) {
    puts("Unspecified input string.\n");
    return 1;
  }

  print_loop:
  col = 0;
  db->hidx = 0;

  clear_buf(db);

  while(s[s_idx] && col < db->hsize - SYMBOL_HSIZE - 1) {
    char c;
    c = s[s_idx];
    push_char(db, c);
    s_idx += 1;
    col = db->hidx;
  }
  print_buf(db);

  if(s[s_idx]) goto print_loop;

  return 0;
}

/* throw away col=0, shift col=1..end-1, col=end becomes \0 */
static int shift_db_left(struct disp_buf *db)
{
  size_t row;

  for(row = 0; row < db->virt_vsize; row++) {
    memmove(db->buf[row], db->buf[row]+1, db->virt_hsize-2);
    db->buf[row][db->virt_hsize-2] = ' ';
    db->buf[row][db->virt_hsize-1] = '\0';
  }

  return 0;
}

int dishscroll(const char *s, const struct disp_buf *dims)
{
  static struct disp_buf *db = NULL;
  static int f_bufexists = 0;
  size_t row, s_idx, col, i;

  s_idx = 0;
  col = 0;

  if(!s && f_bufexists) {
    f_bufexists = 0;
    return _clean_up(db);
  }

  if(!f_bufexists) {
    if(dims)
      db = new_display_buffer(dims->hsize, dims->vsize, 
        dims->delay->tv_nsec);
    else
      db = new_display_buffer(HSIZE, VSIZE, DELAY_NS);

    if(CHKPTR(db)) {
      puts("Could not create display buffer.\n");
      return 1;
    }
    f_bufexists = 1;
  }
  if(CHKPTR(s)) {
    puts("Invalid input str.\n");
    return 1;
  }
  do {
    db->hidx = db->virt_hsize - SYMBOL_WIDTH;
    push_char(db, s[s_idx]);
    db->hidx = db->virt_hsize - SYMBOL_WIDTH;
    for(i = 0; i < SYMBOL_WIDTH; i++) {
      shift_db_left(db);
      print_buf(db);
      nanosleep(db->delay, NULL);
      clear_screen(db);
    }
    s_idx += 1;
  } while(s[s_idx]);
  print_buf(db);
  return 0;
}

static int shift_rows_down(struct disp_buf *db, size_t hstart, size_t hend)
{
  size_t i, hrange;
  hrange = hend - hstart;

  if(hstart >= db->virt_hsize || 
    hend >= db->virt_hsize || 
    hrange > db->virt_hsize ||
    hrange < 1) {
    puts("Invalid column range.\n");
    return -1;
  }

  for(i = db->virt_vsize-1; i > 0; i--) {
    memcpy(db->buf[i] + hstart, db->buf[i-1] + hstart, hrange);
  }
  memset(db->buf[0] + hstart, ' ', hrange);

  return 0;
}

int disflip(const char *s, const struct disp_buf *dims) {
  static struct disp_buf *db = NULL;
  static int f_bufexists = 0;
  size_t content_length, i, j;

  if(!s && f_bufexists) {
    f_bufexists = 0;
    return _clean_up(db);
  }

  if(!f_bufexists)
  {
    size_t s_idx;
    char c;

    s_idx = 0;

    if(dims)
      db = new_display_buffer(dims->hsize, dims->vsize, 
        dims->delay->tv_nsec);
    else
      db = new_display_buffer(HSIZE, VSIZE, DELAY_NS);

    if(CHKPTR(db)) {
      puts("Could not create display buffer.\n");
      return 1;
    }
    f_bufexists = 1;

    if(CHKPTR(s)) {
      puts("Nowt in buffer.\n");
      return 1;
    }

    content_length = (strlen(s) + 1) * sizeof *db->content;
    db->content = malloc(content_length);
    if(CHKPTR(db->content)) {
      perror("malloc");
      return 1;
    }
    memcpy(db->content, s, content_length);
    /* now fill buffer so everything is set up */
    while((c = s[s_idx]) != '\0' && s_idx * SYMBOL_WIDTH < db->hsize) {
      push_char(db, c);
      s_idx += 1;
    }
    print_buf(db);
    return 0;
  }

  if(CHKPTR(s))
    return 0;

  content_length = (strlen(s) + 1) * sizeof *db->content;
  
  if(content_length > strlen(db->content) + 1) {
    db->content = realloc(db->content, content_length);
    memset(db->content + strlen(db->content), '\0',
      content_length - strlen(db->content));
  }

  /* compare new characters to existing content */
  for(i = 0; i < content_length; i++) {
    char current, next;
    current = db->content[i];
    next = s[i];

    if(next == '\0') {
      /* if significantly shorter then release memory */
      if(content_length - i > RELEASE_THRESHOLD) {
        db->content = realloc(db->content, content_length);
      }
      break;
    }

    if(current != next) {
      /* add new symbol above old one */
      size_t x, y;
      x = SYMBOL_HSIZE * i;
      y = 0;
      if(insert_symbol_at(db, char_to_symbol(next), x, y)) {
        puts("Failed to add new symbol.\n");
        return 1;
      }
    }
  }

  for(j = 0; j < db->vsize; j++) {
    for(i = 0; i < content_length; i++) {
      char current, next;
      current = db->content[i];
      next = s[i];

      if(next == '\0')
        break;

      if(current != next) {
        size_t x;
        x = SYMBOL_HSIZE * i;
        if(shift_rows_down(db, x, x + SYMBOL_HSIZE)) {
          puts("Failed to shift rows down.\n");
          return 1;
        }
      }
    }
    clear_screen(db);
    print_buf(db);
    nanosleep(db->delay, NULL);
  }

  memcpy(db->content, s, content_length);


  return 0;
}
