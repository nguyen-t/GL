#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "fmap.h"

#define PAGE_SIZE (double) sysconf(_SC_PAGE_SIZE)

char* fmap(const char* fileName) {
  int fd = open(fileName, O_RDWR);
  int length = lseek(fd, 0, SEEK_END);
  int bytes = 4096 * ceil((length + 1) / PAGE_SIZE);
  char* map;

  if(fd == -1 || length == -1) {
    return NULL;
  }

  if((map = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    return NULL;
  }

  close(fd);

  return map;
}

int funmap(char* file) {
  int length = 4096 * ceil(strlen(file) / 4096.0);
  return munmap(file, length);
}
