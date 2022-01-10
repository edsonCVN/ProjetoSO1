#include "../fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <threads_api.h>
#define COUNT 8
#define SIZE 2500
#define N 8
/*

*/
int main() {

    pthread_t tid[N];
    //tfs_open_paramts open_input[N];
    tfs_write_paramts write_input[2];
    tfs_read_paramts read_input[2];

    char input1[SIZE];
    memset(input1, 'A', SIZE);
    memset(input1, 'X', SIZE - 1);
    char output1[SIZE];

    char input2[SIZE];
    memset(input2, 'A', SIZE);
    memset(input2, 'X', SIZE - 1);
    char output2[SIZE];

    assert(tfs_init() != -1);

    tfs_write_paramts_set(&write_input[0], "/f1", TFS_O_CREAT, input1, SIZE, COUNT);
    pthread_create(&tid[4], NULL, tfs_write_tapi, (void *)&write_input[0]);

    tfs_write_paramts_set(&write_input[1], "/f2", TFS_O_CREAT, input2, SIZE, COUNT);
    pthread_create(&tid[5], NULL, tfs_write_tapi, (void *)&write_input[1]);

    tfs_read_paramts_set(&read_input[0], "/f1", 0, output1, SIZE, COUNT);
    pthread_create(&tid[6], NULL, tfs_read_tapi, (void *)&read_input[0]);


    tfs_read_paramts_set(&read_input[1], "/f2", 0, output2, SIZE, COUNT);
    pthread_create(&tid[7], NULL, tfs_read_tapi, (void *)&read_input[1]);


    for (int i = 4; i < 6; i++) {
        pthread_join(tid[i], NULL);
        printf("write %d: %ld\n",i-4, write_input[i-4].rtn_value);

    }

    for (int i = 6; i < N; i++) {
        pthread_join(tid[i], NULL);
        printf("read %d: %ld\n",i-6, read_input[i-6].rtn_value);
    }
  
    assert(tfs_destroy() != -1);

    return 0;
}