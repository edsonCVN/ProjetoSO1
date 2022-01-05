#include "fs/operations.h"
#include <assert.h>

int main() {
    char *path1 = "/f1";
    int f1;
    /* Tests different scenarios where tfs_copy_to_external_fs is expected to fail */

    assert(tfs_init() != -1);
    
    tfs_open_paramts paramts;
    paramts.pth = path1;
    paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&paramts);
    f1 = paramts.rtn_value;
    assert(f1 != -1);

    /* Scenario 1: destination file is in directory that does not exist */
    assert (tfs_copy_to_external_fs(path1, "./wrong_dir/unexpectedfile") == -1);

    /* Scenario 2: source file does not exist */
    assert(tfs_copy_to_external_fs("/f2", "out") == -1);
    assert(tfs_destroy() != -1);
    printf("Successful test.\n");

    return 0;
}
