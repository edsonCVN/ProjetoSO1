#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 2
#define SIZE 25
#define N 4

int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE);
    memset(input1, 'X', SIZE - 1);
    char output1[SIZE*4];
    
    char input2[SIZE];
    memset(input2, 'B', SIZE);
    memset(input2, 'Y', SIZE - 1);
    char output2[SIZE];
    
    pthread_t tid[N];
    tfs_write_paramts write_input[N/2];
    tfs_write_paramts_set(&write_input[0], "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    tfs_write_paramts_set(&write_input[1], "/f1", TFS_O_APPEND, input2, SIZE, COUNT);
    tfs_read_paramts read_input[N/2];
    tfs_read_paramts_set(&read_input[0], "/f1", 0, output1, SIZE*4, COUNT-1);
    tfs_read_paramts_set(&read_input[1], "/f1", 0, output2, SIZE, COUNT);

    assert(tfs_init() != -1);

    for(int i = 0; i < N / 2; i++) {
        pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]);
        pthread_join(tid[i], NULL);
    }

    //for(int i = N / 2; i < N; i++) {
        pthread_create(&tid[2], NULL, tfs_read_tapi, (void*)&read_input[0]);
    //}

    for(int i = 2; i < N - 1; i++) {
        pthread_join(tid[i], NULL);
    }
    for(int i = 0; i < N / 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
        
    }
    //for(int i = N / 2; i < N - 1; i++) {
       assert(read_input[0].rtn_value == (SIZE*COUNT*2));
    //}
    
    assert(tfs_destroy() != -1);
}   