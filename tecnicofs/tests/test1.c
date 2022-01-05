#include "fs/operations.h"
#include <assert.h>
#include <string.h>

int main() {

    char *str = "AAA!";
    char *path = "/f1";
    char buffer[40];

    assert(tfs_init() != -1);

    int f;
    ssize_t r;
    tfs_open_paramts paramts;
    paramts.pth = path;
    paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&paramts);
    f = paramts.rtn_value;
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    paramts.flg = 0;
    tfs_open((void*)&paramts);
    f = paramts.rtn_value;
    assert(f != -1);

    r = tfs_read(f, buffer, sizeof(buffer) - 1);
    assert(r == strlen(str));

    buffer[r] = '\0';
    assert(strcmp(buffer, str) == 0);

    assert(tfs_close(f) != -1);
    assert(tfs_destroy() != -1);
    printf("Successful test.\n");

    return 0;
}
