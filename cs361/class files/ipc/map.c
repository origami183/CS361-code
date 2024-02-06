#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define LEN 10

typedef enum { PRINT, PLAY, QUIT, BAD } option_t;

option_t game_loop (char *, size_t *, size_t *);
void print_board (char *, size_t);
void play (char *, char, size_t, size_t);

void
run_tictac (void)
{
  printf ("Welcome to tic-tac-toe\n");
  printf ("Commands are:\n");
  printf ("  p       Print the current board status\n");
  printf ("  x 1 1   Place an x at row 1, column 1\n");
  printf ("  o 1 1   Place an o at row 1, column 1\n");
  printf ("  q       Quit\n");

  // Open the file and get stat information
  int fd = open ("tictac", O_RDWR);
  assert (fd >= 0);
  struct stat info;
  assert (fstat (fd, &info) != -1);

  char *board =
    mmap (NULL, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  assert (board != NULL);
  fprintf (stderr, "Board is mapped at %p\n", board);

  char xo;
  size_t row = 0, col = 0;
  while (1)
    {
      option_t opt = game_loop (&xo, &row, &col);
      if (opt == QUIT)
        break;

      if (opt == PRINT)
        print_board (board, info.st_size);
      else if (opt == PLAY)
        {
          play (board, xo, row, col);
          msync (board, info.st_size, MS_SYNC);
        }
    }

  munmap (board, info.st_size);
  close (fd);
}


void
print_board (char *board, size_t size)
{
  for (size_t i = 0; i < size; i++)
    printf ("%c", board[i]);
}

void
play (char *board, char xo, size_t row, size_t col)
{
  // Need to map row and column to an offset. Each row is 14 characters wide,
  // and everything needs to zero-based.
  size_t pos = ((row - 1) * 2 + 1) * 14;
  pos += (col - 1) * 4 + 2;
  fprintf (stderr, "Row %zd, column %zd map to %p\n", row, col, &board[pos]);
  if (board[pos] == ' ')
    board[pos] = xo;
  else
    fprintf (stderr, "Cannot play at %zd %zd\n", row, col);
}

option_t
game_loop (char *xo, size_t *row, size_t *col)
{
  char buffer[LEN];
  memset (buffer, 0, LEN);
  fgets (buffer, LEN - 1, stdin);

  char *newline = strchr (buffer, '\n');
  if (newline != NULL)
    *newline = '\0';

  switch (buffer[0])
    {
      case 'p':
      case 'P':
        return PRINT;

      case 'x':
      case 'X':
      case 'o':
      case 'O':
        *xo = toupper (buffer[0]);
        break;

      case 'q':
      case 'Q':
        return QUIT;

      default:
        fprintf (stderr, "Bad command: %s\n", buffer);
        return BAD;
    }

  char *cont = NULL;
  *row = strtol (&buffer[1], &cont, 10);
  assert (errno != EINVAL);
  *col = strtol (cont, NULL, 10);
  assert (errno != EINVAL);

  if (*row < 1 || *row > 3)
    {
      fprintf (stderr, "Invalid row number in %s\n", buffer);
      return BAD;
    }
  if (*col < 1 || *col > 3)
    {
      fprintf (stderr, "Invalid column number in %s\n", buffer);
      return BAD;
    }
  return PLAY;
}
