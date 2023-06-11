#include "kernel/types.h"
#include "user/user.h"

#define RANDOM_DEV "random"

int main(int argc, char *argv[]) {
  int fd = open(RANDOM_DEV, 0); // Open /random device

  if (fd < 0) {
    printf("Failed to open %s\n", RANDOM_DEV);
    exit(1);
  }

  // Seed the random number generator with value 0x55
  char seed = 0x55;
  write(fd, &seed, 1);

  // Read and print 10 random numbers
  char buffer[10];
  int numRead = read(fd, buffer, sizeof(buffer));

  if (numRead < 0) {
    printf("Read error\n");
    exit(1);
  }

  printf("Random numbers: ");
  for (int i = 0; i < numRead; i++) {
    printf("%02x ", buffer[i]);
  }
  printf("\n");

  close(fd);

  exit(0);
}