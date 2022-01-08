#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>

#define COUNT 8
#define SIZE 25
#define N 4

int main() {

    char input1[SIZE];
    memset(input1, 'A', SIZE);
    memset(input1, 'X', SIZE - 1);
    char output1[SIZE];
    
    char input2[SIZE];
    memset(input2, 'B', SIZE);
    memset(input2, 'Y', SIZE - 1);
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
        pthread_create(&tid[i], NULL, tfs_write_tapi, (void*)&write_input[i]);
    }

    for(int i = N / 2; i < N; i++) {
        pthread_create(&tid[i], NULL, tfs_read_tapi, (void*)&read_input[i-2]);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }
    for(int i = 0; i < N / 2; i++) {
        assert(write_input[i].rtn_value == (SIZE*COUNT));
        
    }
    for(int i = N / 2; i < N; i++) {
       assert(read_input[i-2].rtn_value == (SIZE*COUNT));
    }
    
    assert(tfs_destroy() != -1);
}   