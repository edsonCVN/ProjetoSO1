#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 2
#define SIZE 25
/*
    Ourtest - 3 threads: 2 Write 1 Read

    Neste teste lançamos três threads. As duas primeiras abrem o ficheiro "/f1" em simultâneo, porém uma abre com 
    a flag TFS_O_CREAT e a outra com TFS_O_APPEND e escrevem o conteúdo dos inputs respetivos COUNT vezes. A terceira 
    tarefa abre o ficheiro "/f1" com a flag 0 e escreve, duma vez, no output1 o que foi escrito na totalidade pelas 
    duas threads anteriores.

    Objetivo: Testar a escrita em paralelo no mesmo ficheiro.
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
    tfs_read_paramts_set(&read_input, "/f1", 0, output1, SIZE*2, 1);

    assert(tfs_init() != -1);

    /* Lançamento das threads que fazem a escrita em "/f1". */
    for(int i = 0; i < 2; i++) {
        pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]);
    }

    /* Lançamento da thread responsável pela leitura de "/f1". */
    pthread_create(&tid[2], NULL, tfs_read_tapi, (void*)&read_input);

    for(int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    for(int i = 0; i < 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
    }

    assert(read_input.rtn_value == (SIZE*2));
 
    assert(tfs_destroy() != -1);

    printf("Successful test.\n");
}   