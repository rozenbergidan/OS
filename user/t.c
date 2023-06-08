#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define SEEK_SET 0
#define SEEK_CUR 1

int main()
{
    // Create the testfile.txt
    int fd = open("testfile.txt", O_WRONLY | O_CREATE);
    if (fd < 0)
    {
        printf("Failed to create the file.\n");
        exit(-1);
    }

    // Write some content to the file
    char content[] = "this is a text test we want to see if seek is working.";
    write(fd, content, sizeof(content) - 1);

    // Close the file
    close(fd);

    // Reopen the file for reading
    fd = open("testfile.txt", O_RDONLY);
    if (fd < 0)
    {
        printf("Failed to open the file.\n");
        exit(-1);
    }

    // Perform seek operations
    int offset = 10;
    int whence = SEEK_SET;
    int ret = seek(fd, offset, whence);
    printf("Seek operation 1 returned: %d\n", ret);
    char resultSeekContent[10];
    read(fd, &resultSeekContent, 10);
    printf("Seek operation 1 result: %s\n", resultSeekContent);

    offset = -5;
    ret = seek(fd, offset, whence);
    printf("Seek operation 2 returned: %d\n", ret);
    read(fd, &resultSeekContent, 10);
    printf("Seek operation 1 result: %s\n", resultSeekContent);

    offset = 7;
    whence = SEEK_CUR;
    ret = seek(fd, offset, whence);
    printf("Seek operation 3 returned: %d\n", ret);
    read(fd, &resultSeekContent, 10);
    printf("Seek operation 1 result: %s\n", resultSeekContent);

    // Close the file
    close(fd);

    exit(0);
}
