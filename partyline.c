#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "partyline.h"

#define ESC 0x1b

static struct termios orig;

static void teardown(void);
static void exit_with_error(const char *msg);

static int disable_raw_mode(int fd) { return tcsetattr(fd, TCSAFLUSH, &orig); }

static int enable_raw_mode(int fd) {
  struct termios raw;
  if (tcgetattr(fd, &orig) == -1) return -1;
  atexit(teardown);

  raw = orig;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN]  = 1;
  raw.c_cc[VTIME] = 0;

  return tcsetattr(fd, TCSAFLUSH, &raw);
}

enum {
  KEY_NULL      = 0,
  KEY_CTRL_C    = 3,
  KEY_BACKSPACE = 127,
  ARROW_LEFT    = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  KEY_DELETE,
  KEY_HOME,
  KEY_END,
  KEY_PAGE_UP,
  KEY_PAGE_DOWN,
  KEY_EOF,
};

static int read_keypress(int fd) {
  char    c;
  ssize_t n;

  n = read(fd, &c, 1);

  if (n == 0) return KEY_EOF; // real EOF
  if (c == 4) return KEY_EOF; // ctrl + d
  if (n == -1 && errno != EAGAIN) exit_with_error("Failed to read input");
  if (c != ESC) return c;

  char seq[3];
  if (read(fd, &seq[0], 1) != 1) return ESC;
  if (read(fd, &seq[1], 1) != 1) return ESC;

  if (seq[0] == '[' || seq[0] == 'O') {
    if (seq[1] >= '0' && seq[1] <= '9') {
      if (read(fd, &seq[2], 1) != 1) return ESC;
      switch (seq[1]) {
        case '1': return KEY_HOME;
        case '3': return KEY_DELETE;
        case '4': return KEY_END;
        case '5': return KEY_PAGE_UP;
        case '6': return KEY_PAGE_DOWN;
      }
    } else {
      switch (seq[1]) {
        case 'A': return ARROW_UP;
        case 'B': return ARROW_DOWN;
        case 'C': return ARROW_RIGHT;
        case 'D': return ARROW_LEFT;
        case 'H': return KEY_HOME;
      }
    }
  }

  return ESC;
}

static void handle_interrupts(int sig) {
  (void)sig;
  exit_with_error("Received interrupt signal");
}

static void setup(void) {
  signal(SIGINT, handle_interrupts);
  signal(SIGTERM, handle_interrupts);

  if (enable_raw_mode(STDIN_FILENO) == -1)
    exit_with_error("Failed to enable raw mode");
}

static void teardown(void) {
  if (disable_raw_mode(STDIN_FILENO) == -1) {
    perror("Failed to disable raw mode");
  }
}

static void exit_with_error(const char *msg) {
  perror(msg);
  teardown();
  exit(EXIT_FAILURE);
}

char *partyline(const char *prompt) {
  setup();

  char  *buf    = NULL;
  size_t buflen = 0, pos = 0;

  write(STDOUT_FILENO, prompt, strlen(prompt));

  int key;
  while ((key = read_keypress(STDIN_FILENO)) != '\r') {
    if (key == KEY_EOF || key == KEY_CTRL_C) {
      write(STDOUT_FILENO, "\r\n", 2);
      teardown();
      return NULL;
    } else if (key >= ' ' && key <= '~') {
      // TODO: better dynamic buffer implementation to avoid realloc in hot loop
      buf = realloc(buf, buflen + 2);
      memmove(buf + pos + 1, buf + pos, buflen - pos);
      buf[pos] = key;
      buflen++;

      size_t suffix = buflen - pos;
      write(STDOUT_FILENO, buf + pos, suffix);

      for (size_t i = 0; i < suffix - 1; i++)
        write(STDOUT_FILENO, "\b", 1);

      pos++;
    } else if (key == KEY_BACKSPACE && pos > 0) {
      memmove(buf + pos - 1, buf + pos, buflen - pos);
      buflen--;
      pos--;

      write(STDOUT_FILENO, "\b", 1);

      size_t tail = buflen - pos;
      write(STDOUT_FILENO, buf + pos, tail);
      write(STDOUT_FILENO, " ", 1);

      for (size_t i = 0; i < tail + 1; i++)
        write(STDOUT_FILENO, "\b", 1);
    } else if (key == KEY_DELETE && pos < buflen) {
      memmove(buf + pos, buf + pos + 1, buflen - pos - 1);
      buflen--;

      size_t tail = buflen - pos;
      write(STDOUT_FILENO, buf + pos, tail);
      write(STDOUT_FILENO, " ", 1);

      for (size_t i = 0; i < tail + 1; i++)
        write(STDOUT_FILENO, "\b", 1);
    } else if (key == ARROW_LEFT && pos > 0) {
      write(STDOUT_FILENO, "\b", 1);
      pos--;
    } else if (key == ARROW_RIGHT && pos < buflen) {
      write(STDOUT_FILENO, &buf[pos], 1);
      pos++;
    } else if (key == KEY_HOME) {
      while (pos > 0) {
        write(STDOUT_FILENO, "\b", 1);
        pos--;
      }
    } else if (key == KEY_END) {
      size_t tail = buflen - pos;
      write(STDOUT_FILENO, buf + pos, tail);
      pos = buflen;
    }
  }

  write(STDOUT_FILENO, "\r\n", 2);

  buf         = realloc(buf, buflen + 1);
  buf[buflen] = '\0';
  teardown();
  return buf;
}
