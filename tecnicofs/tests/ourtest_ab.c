#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 2
#define SIZE 25
#define N 4
/*
    Neste teste lançamos três threads. As duas primeiras abrem o ficheiro "/f1" porém uma abre com a flag TFS_O_CREAT
    e a outra com TFS_O_APPEND e escrevem o conteúdo dos inputs respetivos COUNT vezes. A tarefa restante
    abre o ficheiro "/f1" com a flag 0 e escreve, duma vez, no output1 o que supostamete foi escrito na totalidade
    pelas duas threads anteriores.

    Nota: verificar em threads_api.c:54 o que o tamanho do buffer de leitura corresponde ao tamanho do print
*/
int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE);
    memset(input1, 'X', SIZE - 1);
    char output1[SIZE*4];
    
    char input2[SIZE];
    memset(input2, 'B', SIZE);
    memset(input2, 'Y', SIZE - 1);
    //char output2[SIZE];
    
    pthread_t tid[N-1];
    tfs_write_paramts write_input[N/2];
    tfs_write_paramts_set(&write_input[0], "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    tfs_write_paramts_set(&write_input[1], "/f1", TFS_O_APPEND, input2, SIZE, COUNT);
    tfs_read_paramts read_input[1];
    tfs_read_paramts_set(&read_input[0], "/f1", 0, output1, SIZE*4, COUNT-1);


    assert(tfs_init() != -1);

    for(int i = 0; i < N / 2; i++) {
        pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]);
        
    }

    //for(int i = N / 2; i < N; i++) {
        pthread_create(&tid[2], NULL, tfs_read_tapi, (void*)&read_input[0]);
    //}

    for(int i = 0; i < N - 1; i++) {
        pthread_join(tid[i], NULL);
    }
    for(int i = 0; i < N / 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
        
    }
    //for(int i = N / 2; i < N - 1; i++) {
       assert(read_input[0].rtn_value == (SIZE*4));
    //}
    
    assert(tfs_destroy() != -1);
}   