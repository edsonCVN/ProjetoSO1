/*                      
 *                     Projeto de Sistemas Operativos 2021-22
 *                         1º exercício LEIC-A/LEIC-T/LETI 
 *                        
 *                                Grupo 17 - LETI
 *                    Pedro Alexandre Delgado Claro ist198960
 *                   Edson Fernando Cabral da Veiga ist1100731
 * 
 */

#ifndef THREADS_API_H
#define THREADS_API_H

#include "config.h"
#include "state.h"

/* 
 * Estrutura utilizada para a passagem dos argumentos da função tfs_write_paramts_tapi 
 * (argumentos para chamar tfs_open e chamar tfs_write 'count' vezes) e
 * para obtenção dos valores de retorno obtidos dentro da mesma (provenientes de tfs_open e tfs_write).
 *
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

/* 
 * Estrutura utilizada para a passagem dos argumentos da função tfs_read_paramts_tapi 
 * (argumentos para chamar tfs_open e chamar tfs_read 'count' vezes) e
 * para obtenção dos valores de retorno obtidos dentro da mesma (provenientes de tfs_open e tfs_read).
 *
 */
typedef struct {
    char const *pth;    
    int flg;
    int fhandle;
    void *buffer;
    size_t len;
    int count;
    ssize_t rtn_value;
} tfs_read_paramts;

/* 
 * Estrutura utilizada para a passagem dos argumentos da função tfs_copy_to_external_paramts_tapi 
 * (argumentos para chamar tfs_copy_to_external) e
 * para obtenção do valor de retorno obtido dentro da mesma (proveniente de tfs_copy_to_external).
 *
 */
typedef struct {
    char const *pth;    
    int flg;
    char const *src_path;
    char const *dest_path;
    int rtn_value;
} tfs_copy_to_external_paramts;


/* 
 * Funcao para passagem dos parâmetros para uma estrutura tfs_write_paramts.
 */
void tfs_write_paramts_set (tfs_write_paramts *paramts, char const *pth, int flg, void const *buffer, size_t to_write, int count);

/* 
 * Funcao para passagem dos parâmetros para uma estrutura tfs_read_paramts.
 */
void tfs_read_paramts_set (tfs_read_paramts *paramts, char const *pth, int flg, void *buffer, size_t len, int count);

/* 
 * Funcao para passagem dos parâmetros para uma estrutura tfs_copy_to_external_paramts.
 */
void tfs_copy_to_external_paramts_set(tfs_copy_to_external_paramts *paramts, char const *pth, int flg, char const *src_path, char const *dest_path);


/* 
 * Funcao chamada pela thread para executar a escrita em um ficheiro utilizando ponteiro tfs_write_paramts*.
 */
void *tfs_write_tapi (void *arg);

/* 
 * Funcao chamada pela thread para executar a leitura de um ficheiro utilizando ponteiro tfs_read_paramts*.
 */
void *tfs_read_tapi(void *arg);

/* 
 * Funcao chamada pela thread para executar a cópia de um ficheiro para um sistema exterior utilizando ponteiro tfs_copy_to_external_paramts*.
 */ 
void *tfs_copy_to_external_fs_tapi(void *arg);

#endif // THREADS_API_H