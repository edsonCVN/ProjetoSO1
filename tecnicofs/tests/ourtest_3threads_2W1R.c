/*                      
 *                     Projeto de Sistemas Operativos 2021-22
 *                         1º exercício LEIC-A/LEIC-T/LETI 
 *                        
 *                                Grupo 17 - LETI
 *                    Pedro Alexandre Delgado Claro ist198960
 *                   Edson Fernando Cabral da Veiga ist1100731
 * 
 */
#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 1
#define SIZE 50
/*
    Ourtest - 3 threads: 2 Write 1 Read

    Neste teste lançamos três threads. As duas primeiras escrevem escrevem o conteúdo dos inputs, 
    no mesmo ficheiro "/f1" em simultâneo. A terceira thread efectua a leitura do ficheiro "/f1".

    Objetivo: Testar a escrita e leitura em paralelo no mesmo ficheiro.
*/
int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE - 1);
    memset(input1 + (SIZE - 1), 'X', 1);
    
    char input2[SIZE];
    memset(input2, 'B', SIZE - 1);
    memset(input2 + (SIZE - 1), 'Y', 1);

    char output1[SIZE*2];

    pthread_t tid[3];
    tfs_write_paramts write_input[2];
    tfs_write_paramts_set(&write_input[0], "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    tfs_write_paramts_set(&write_input[1], "/f1", TFS_O_CREAT, input2, SIZE, COUNT);
    tfs_read_paramts read_input;
    tfs_read_paramts_set(&read_input, "/f1", 0, output1, SIZE, COUNT);

    assert(tfs_init() != -1);

    /* Lançamento das threads que fazem a escrita em "/f1". */
    for(int i = 0; i < 2; i++) {
        assert(pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]) == 0);
    }

    /* Lançamento da thread responsável pela leitura de "/f1". */
    assert(pthread_create(&tid[2], NULL, tfs_read_tapi, (void*)&read_input) == 0);

    for(int i = 0; i < 3; i++) {
        assert(pthread_join(tid[i], NULL) == 0);
    }

    for(int i = 0; i < 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
    }

    assert(read_input.rtn_value == (SIZE));
    
    assert(strncmp(input1, output1, SIZE) == 0 || strncmp(input2, output1, SIZE) == 0);

    assert(tfs_destroy() != -1);

    printf("Successful test.\n");
}   