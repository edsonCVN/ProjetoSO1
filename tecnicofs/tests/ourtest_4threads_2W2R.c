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

#define COUNT 8
#define SIZE 25
#define N 4
/*
    Ourtest - 4 threads: 2 Write 2 Read

    Neste teste lançamos 4 threads: duas criam "/f1" e "/f2" e escrevem em cada um SIZE*COUNT bytes e 
    ao mesmo tempo outras duas lêem de "/f1" e "/f2" SIZE*COUNT , cada.

    Objetivo: Testar o paralelismo na escrita e leitura em paralelo de ficheiros diferentes.
*/
int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE - 1);
    memset(input1 + (SIZE - 1), 'X', 1);
    char output1[SIZE];
    
    char input2[SIZE];

    memset(input2, 'B', SIZE - 1);
    memset(input2 + (SIZE - 1), 'Y', 1);
    char output2[SIZE];
         
    pthread_t tid[N];
    tfs_write_paramts write_input[N/2];
    tfs_write_paramts_set(&write_input[0], "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    tfs_write_paramts_set(&write_input[1], "/f2", TFS_O_CREAT, input2, SIZE, COUNT);
    tfs_read_paramts read_input[N/2];
    tfs_read_paramts_set(&read_input[0], "/f1", 0, output1, SIZE, COUNT);
    tfs_read_paramts_set(&read_input[1], "/f2", 0, output2, SIZE, COUNT);

    assert(tfs_init() != -1);

    for(int i = 0; i < N / 2; i++) {
        assert(pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]) == 0);
    }

    for(int i = N / 2; i < N; i++) {
        assert(pthread_create(&tid[i], NULL, tfs_read_tapi, (void*)&read_input[i-2]) == 0);
    }

    for(int i = 0; i < N; i++) {
        assert(pthread_join(tid[i], NULL) == 0);
    }

    for(int i = 0; i < N / 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
        
    }
    
    for(int i = N / 2; i < N; i++) {
       assert(read_input[i-2].rtn_value == (SIZE*COUNT));
    }
    
    assert(strncmp(input1, output1, SIZE) == 0);

    assert(strncmp(input2, output2, SIZE) == 0);

    assert(tfs_destroy() != -1);

    printf("Successful test.\n");
}   