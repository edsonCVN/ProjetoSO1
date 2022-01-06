#ifndef THREADS_API_H
#define THREADS_API_H

#include "config.h"
#include "state.h"

typedef struct {
    char const *pth;    
    int flg;
    int rtn_value;
} tfs_open_paramts;

typedef struct {
    int fhandle; 
    void const *buffer; 
    size_t to_write;
    ssize_t rtn_value;
} tfs_write_paramts;

typedef struct {
    int fhandle;
    void *buffer;
    size_t len;
    ssize_t rtn_value;
} tfs_read_paramts;

typedef struct {
    char const *src_path;
    char const *dest_path;
    int rtn_value;
} tfs_copy_to_external_paramts;

/*
 * Opens a file
 * 
 * INCOMPLETO
 */
void *tfs_open_tapi(void *arg);

/* Writes to an open file, starting at the current offset
 * 
    INCOMPLETO
 */
void *tfs_write_tapi (void *arg);

/* Reads from an open file, starting at the current offset
 * INCOMPLETO
 */
void *tfs_read_tapi(void *arg);

/* Copies the contents of a file that exists in TecnicoFS to the contents
 * INCOMPLETO
*/ 
void *tfs_copy_to_external_fs_tapi(void *arg);

#endif // THREADS_API_H