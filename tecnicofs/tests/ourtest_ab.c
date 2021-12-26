#include "../fs/operations.h"
#include <assert.h>
#include <string.h>

#define COUNT 109 // a partir de = 109 ele fica em loop infinito (é quando ultrapassa o tamamnho máximo 272384)
                  // este ciclo infinito ocorre pelo menos no write...
#define SIZE 2500

/**
   This test fills in a new file up to 20 blocks via multiple writes
   (therefore causing the file to hold 10 direct references + 10 indirect
   references from a reference block),
   each write always targeting only 1 block of the file, 
   then checks if the file contents are as expected
 */


int main() {

    char *path = "/f1";

    /* Writing this buffer multiple times to a file stored on 1KB blocks will 
       always hit a single block (since 1KB is a multiple of SIZE=256) */
    char input[SIZE]; 
    memset(input, 'A', SIZE);

    char output [SIZE];
    ssize_t size = 0;
    ssize_t total_size = 0;
    assert(tfs_init() != -1);

    /* Write input COUNT times into a new file */
    int fd = tfs_open(path, TFS_O_CREAT);
    assert(fd != -1);
    for (int i = 0; i < COUNT; i++) {
        size = tfs_write(fd, input, SIZE);
        total_size += size;
        if(total_size < 272384)
            assert(size == SIZE);
        else break;
    }
    assert(tfs_close(fd) != -1);

    /* Open again to check if contents are as expected */
    fd = tfs_open(path, 0);
    assert(fd != -1 );


    size = 0;
    total_size = 0;
    for (int i = 0; i < COUNT; i++) {
        size = tfs_read(fd, output, SIZE);
        total_size += size;
        if(total_size < 272384) {
             printf("%d\n",i);
             assert(size == SIZE);
        }
        else {assert (memcmp(input, output, (size_t) size) == 0);printf("%ld\t%ld\n",size, total_size);break;}

        assert (memcmp(input, output, (size_t) size) == 0);

    }

    assert(tfs_close(fd) != -1);


    printf("Sucessful test\n");

    return 0;
}