#include "../fs/operations.h"
#include <assert.h>
#include <string.h>

#define COUNT 80
#define SIZE 256

/**
   This test fills in a new file up to 20 blocks via multiple writes
   (therefore causing the file to hold 10 direct references + 10 indirect
   references from a reference block),
   each write always targeting only 1 block of the file, 
   then checks if the file contents are as expected
 */


int main() {

    char *path = "/f1";
    int fd;
    /* Writing this buffer multiple times to a file stored on 1KB blocks will 
       always hit a single block (since 1KB is a multiple of SIZE=256) */
    char input[SIZE]; 
    memset(input, 'A', SIZE);

    char output [SIZE];

    assert(tfs_init() != -1);

    /* Write input COUNT times into a new file */
    tfs_open_paramts paramts;
    paramts.pth = path;
    paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&paramts);
    fd = paramts.rtn_value;
    assert(fd != -1);
    for (int i = 0; i < COUNT; i++) {
        assert(tfs_write(fd, input, SIZE) == SIZE);
    }
    assert(tfs_close(fd) != -1);

    /* Open again to check if contents are as expected */
    paramts.flg = 0;
    tfs_open((void*)&paramts);
    fd = paramts.rtn_value;
    assert(fd != -1);

    for (int i = 0; i < COUNT; i++) {
        assert(tfs_read(fd, output, SIZE) == SIZE);
        assert (memcmp(input, output, SIZE) == 0);
    }

    assert(tfs_close(fd) != -1);
    assert(tfs_destroy() != -1);

    printf("Sucessful test\n");

    return 0;
}