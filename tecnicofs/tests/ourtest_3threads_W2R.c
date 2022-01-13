#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 8
#define SIZE 2500

/*
     Ourtest - 3 threads: 1 Write 2 Read

    Neste teste lançamos 3 threads: a primeira cria o ficheiro "/f1" e escreve SIZE*COUNT bytes e 
    ao mesmo tempo outras duas threads lêem o mesmo ficheiro "/f1".

    Objetivo: Testar a leitura em paralelo do mesmo ficheiros, e o bloqueio de outras threads ao tentarem
    aceder um ficheiro enquanto eh escrito.
*/
int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE - 1);
    memset(input1 + (SIZE - 1), 'X', 1);
    
    char output1[SIZE];
    char output2[SIZE];
    
    pthread_t tid[3];
    tfs_write_paramts write_input;
    tfs_write_paramts_set(&write_input, "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    tfs_read_paramts read_input[2];
    tfs_read_paramts_set(&read_input[0], "/f1", 0, output1, SIZE, COUNT);
    tfs_read_paramts_set(&read_input[1], "/f1", 0, output2, SIZE, COUNT);

    assert(tfs_init() != -1);

    pthread_create(&tid[0], NULL, tfs_write_tapi, (void*)&write_input);

    pthread_join(tid[0], NULL); // confirmar a necessidade de fazer o join antes de chamar as leituras
    
    for(int i = 1; i < 3; i++) {
        pthread_create(&tid[i], NULL, tfs_read_tapi, (void*)&read_input[i-1]);
    }

    for(int i = 1; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    assert(write_input.rtn_value == (SIZE*COUNT)); 
   
    for(int i = 1; i < 3; i++) {
       assert(read_input[i-1].rtn_value == (SIZE*COUNT));
    }
    
    assert(tfs_destroy() != -1);

    printf("Successful test.\n");
}   