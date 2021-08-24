#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int BYTES = 10*1000*1000;

#define BUFSIZE 4096
char buf[BUFSIZE];

int main(int argc, char *argv[]) {
  int res;

  int pipefds[2];
  res = pipe(pipefds);
  if (res < 0) {
    printf("pipetest: pipe() failed\n");
    exit(1);
  }

  res = fork();
  if (res < 0) {
    printf("pipetest: fork() failed\n");
    exit(1);
  }

  if (res == 0) {
    // child
    close(pipefds[0]);
    int bytes_written = 0;
    do {
      int remaining = BYTES - bytes_written;
      int to_write = (remaining < BUFSIZE) ? remaining : BUFSIZE;
      res = write(pipefds[1], buf, to_write);
      if (res < 1) {
        printf("pipetest: child can't write() to pipe\n");
        exit(1);
      }
      bytes_written += res;
    } while (bytes_written < BYTES);
    exit(0);
  } else {
    // parent
    close(pipefds[1]);
    int bytes_read = 0;
    int start = uptime();
    while (1) {
      res = read(pipefds[0], buf, BUFSIZE);
      if (res < 0) {
        printf("pipetest: read() failed in parent\n");
        exit(1);
      }
      if (res == 0)
        break;
      bytes_read += res;
    }
    wait(0);
    int stop = uptime();
    printf("elapsed time = %d\n", stop - start);
    printf("parent exiting after reading %d bytes\n", bytes_read);
    exit(0);
  }
}
