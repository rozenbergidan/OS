#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define RANDOM_DEV "random"

int main(int argc, char *argv[]) {
  int fd = open(RANDOM_DEV, O_RDWR); // Open /random device

  if (fd < 0) {
    printf("Failed to open %s\n", RANDOM_DEV);
    exit(1);
  }

  // Seed the random number generator with value 0x55
  char seed = 0x55;
  int res = write(fd, &seed, 1);
  printf("write result: %d\n", res);

  // Read and print 10 random numbers
  char buffer[8];
  int numRead = read(fd, buffer, 10);

  if (numRead < 0) {
    printf("Read error\n");
    exit(1);
  }



  printf("Random numbers[%d]: ", numRead);
  for (int i = 0; i < numRead; i++) {
    printf("%d ", buffer[i]);
  }
  printf("\n");

  close(fd);
  printf("done\n");

  exit(0);
}