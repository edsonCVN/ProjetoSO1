#include "fs/operations.h"
#include <assert.h>

int main() {
    char *path1 = "/f1";
    int f1;
    tfs_open_paramts open_paramts;
    tfs_copy_to_external_paramts copy_paramts;
    /* Tests different scenarios where tfs_copy_to_external_fs is expected to fail */

    assert(tfs_init() != -1);
    
    open_paramts.pth = path1;
    open_paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&open_paramts);
    f1 = open_paramts.rtn_value;
    assert(f1 != -1);
    
    /* Scenario 1: destination file is in directory that does not exist */
    copy_paramts.src_path = path1; 
    copy_paramts.dest_path = "./wrong_dir/unexpectedfile";
    tfs_copy_to_external_fs((void*)&copy_paramts);
    assert(copy_paramts.rtn_value== -1);

    /* Scenario 2: source file does not exist */
    copy_paramts.src_path = "/f2"; 
    copy_paramts.dest_path = "out";
    tfs_copy_to_external_fs((void*)&copy_paramts);
    assert(copy_paramts.rtn_value== -1);
    
    assert(tfs_destroy() != -1);
    printf("Successful test.\n");

    return 0;
}
