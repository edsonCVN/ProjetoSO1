#include "fs/operations.h"
#include <assert.h>
#include <string.h>

int main() {

    char *str = "AAA!";
    char *path = "/f1";
    char buffer[40];

    assert(tfs_init() != -1);

    int f;
    tfs_open_paramts open_paramts;
    tfs_write_paramts write_paramts;
    tfs_read_paramts read_paramts;
    
    open_paramts.pth = path;
    open_paramts.flg = TFS_O_CREAT;

    tfs_open((void*)&open_paramts);
    f = open_paramts.rtn_value;
    assert(f != -1);
   
    write_paramts.fhandle = f;
    write_paramts.buffer = str;
    write_paramts.to_write = strlen(str);
    
    tfs_write((void*)&write_paramts);
    assert(write_paramts.rtn_value == strlen(str));

    assert(tfs_close(f) != -1);

    open_paramts.flg = 0;
    tfs_open((void*)&open_paramts);
    f = open_paramts.rtn_value;
    assert(f != -1);

    read_paramts.fhandle = f;
    read_paramts.buffer = buffer;
    read_paramts.len = sizeof(buffer) - 1;
    
    tfs_read((void*)&read_paramts);
    assert(read_paramts.rtn_value == strlen(str));

    buffer[read_paramts.rtn_value] = '\0';
    assert(strcmp(buffer, str) == 0);

    assert(tfs_close(f) != -1);
    assert(tfs_destroy() != -1);
    printf("Successful test.\n");

    return 0;
}
