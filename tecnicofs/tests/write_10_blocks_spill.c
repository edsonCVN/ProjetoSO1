#include "../fs/operations.h"
#include <assert.h>
#include <string.h>

#define COUNT 40
#define SIZE 250

/**
   This test fills in a new file up to 10 blocks via multiple writes, 
   where some calls to tfs_write may imply filling in 2 consecutive blocks, 
   then checks if the file contents are as expected
 */


int main() {

    char *path = "/f1";

    /* Writing this buffer multiple times to a file stored on 1KB blocks will 
       sometimes target 2 consecutive blocks (since 1KB is *not* a multiple of SIZE=250) 
    */
    char input[SIZE]; 
    memset(input, 'A', SIZE);
    int fd;
    char output [SIZE];
    tfs_open_paramts open_paramts;
    tfs_write_paramts write_paramts;
    tfs_read_paramts read_paramts;

    assert(tfs_init() != -1);

    /* Write input COUNT times into a new file */


    open_paramts.pth = path;
    open_paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&open_paramts);
    fd = open_paramts.rtn_value;
    assert(fd != -1);

    write_paramts.fhandle = fd;
    write_paramts.buffer = input;
    write_paramts.to_write = SIZE;
    for (int i = 0; i < COUNT; i++) {
        tfs_write((void*)&write_paramts);
        assert(write_paramts.rtn_value == SIZE);
    }
    assert(tfs_close(fd) != -1);

    /* Open again to check if contents are as expected */
    open_paramts.flg = 0;
    tfs_open((void*)&open_paramts);
    fd = open_paramts.rtn_value;
    assert(fd != -1);

    read_paramts.fhandle = fd;
    read_paramts.buffer = output;
    read_paramts.len = SIZE;
    for (int i = 0; i < COUNT; i++) {
        tfs_read((void*)&read_paramts);
        assert(read_paramts.rtn_value == SIZE);
        assert (memcmp(input, output, SIZE) == 0);
    }

    assert(tfs_close(fd) != -1);
    assert(tfs_destroy() != -1);

    printf("Sucessful test\n");

    return 0;
}