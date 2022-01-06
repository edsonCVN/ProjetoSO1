#include "threads_api.h"
#include "operations.h"


void *tfs_open_tapi(void *arg){
    tfs_open_paramts *paramts = (tfs_open_paramts *)arg;

    paramts->rtn_value = tfs_open(paramts->pth, paramts->flg);
    return NULL;
}

void *tfs_write_tapi (void *arg){
    tfs_write_paramts *paramts = (tfs_write_paramts *)arg;
    
    paramts->rtn_value = tfs_write(paramts->fhandle, paramts->buffer, paramts->to_write);
    return NULL;
}

void *tfs_read_tapi(void *arg){
    tfs_read_paramts *paramts = (tfs_read_paramts *)arg;

    paramts->rtn_value = tfs_read(paramts->fhandle, paramts->buffer, paramts->len);
    return NULL;
}

void *tfs_copy_to_external_fs_tapi(void *arg){
    tfs_copy_to_external_paramts *paramts = (tfs_copy_to_external_paramts *)arg;
    
    paramts->rtn_value = tfs_copy_to_external_fs(paramts->src_path, paramts->dest_path);
    return NULL;
}