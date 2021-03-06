/*                      
 *                     Projeto de Sistemas Operativos 2021-22
 *                         1º exercício LEIC-A/LEIC-T/LETI 
 *                        
 *                                Grupo 17 - LETI
 *                    Pedro Alexandre Delgado Claro ist198960
 *                   Edson Fernando Cabral da Veiga ist1100731
 * 
 */

#include "threads_api.h"
#include "operations.h"
#include <assert.h>
#include <string.h>

void *tfs_write_tapi (void *arg){
    tfs_write_paramts *paramts = (tfs_write_paramts *)arg;
    paramts->fhandle = tfs_open(paramts->pth, paramts->flg);
    assert(paramts->fhandle != -1);

    ssize_t single_rtn_value;

    for (int i = 0; i < paramts->count; i++) {
        single_rtn_value = tfs_write(paramts->fhandle, paramts->buffer, paramts->to_write);
        paramts->rtn_value += single_rtn_value;
    }
    assert(tfs_close(paramts->fhandle) != -1);
    return NULL;
}

void tfs_write_paramts_set (tfs_write_paramts *paramts, char const *pth, int flg, void const *buffer, size_t to_write, int count) {
    paramts->pth = pth;
    paramts->flg= flg;
    paramts->buffer = buffer;
    paramts->to_write = to_write;
    paramts->count = count;
    paramts->rtn_value = 0;
}

void *tfs_read_tapi(void *arg){
    tfs_read_paramts *paramts = (tfs_read_paramts *)arg;
    paramts->fhandle = tfs_open(paramts->pth, paramts->flg);
    assert(paramts->fhandle != -1);
    ssize_t single_rtn_value;
    for (int i = 0; i < paramts->count; i++) {
        single_rtn_value = tfs_read(paramts->fhandle, paramts->buffer, paramts->len);
        paramts->rtn_value += single_rtn_value;
    }
    assert(tfs_close(paramts->fhandle) != -1);
    return NULL;
}

void tfs_read_paramts_set (tfs_read_paramts *paramts, char const *pth, int flg, void  *buffer, size_t len, int count) {
    paramts->pth = pth;
    paramts->flg= flg;
    paramts->buffer = buffer;
    paramts->len = len;
    paramts->count = count;
    paramts->rtn_value = 0;
}

void *tfs_copy_to_external_fs_tapi(void *arg){
    tfs_copy_to_external_paramts *paramts = (tfs_copy_to_external_paramts *)arg;
    paramts->rtn_value = tfs_copy_to_external_fs(paramts->src_path, paramts->dest_path);
    return NULL;
}

void tfs_copy_to_external_paramts_set(tfs_copy_to_external_paramts *paramts, char const *pth, int flg, char const *src_path, char const *dest_path) {
    paramts->pth = pth;
    paramts->flg= flg;
    paramts->src_path = src_path;
    paramts->dest_path = dest_path;
    paramts->rtn_value = -2; 
}

