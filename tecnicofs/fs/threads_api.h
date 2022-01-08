#ifndef THREADS_API_H
#define THREADS_API_H

#include "config.h"
#include "state.h"
/*
typedef struct {
    char const *pth;    
    int flg;
    int rtn_value;
} tfs_open_paramts;
*/
typedef struct {
    char const *pth;    
    int flg;
    int fhandle; 
    void const *buffer; 
    size_t to_write;
    int count;
    ssize_t rtn_value;
} tfs_write_paramts;

typedef struct {
    char const *pth;    
    int flg;
    int fhandle;
    void *buffer;
    size_t len;
    int count;
    ssize_t rtn_value;
} tfs_read_paramts;

typedef struct {
    char const *pth;    
    int flg;
    char const *src_path;
    char const *dest_path;
    int rtn_value;
} tfs_copy_to_external_paramts;

//void tfs_open_paramts_set(tfs_open_paramts *paramts, char const *pth, int flg);

void tfs_write_paramts_set (tfs_write_paramts *paramts, char const *pth, int flg, void const *buffer, size_t to_write, int count);

void tfs_read_paramts_set (tfs_read_paramts *paramts, char const *pth, int flg, void *buffer, size_t len, int count);

void tfs_copy_to_external_paramts_set(tfs_copy_to_external_paramts *paramts, char const *pth, int flg, char const *src_path, char const *dest_path);


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