#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#define COUNT 109 // a partir de = 109 ele fica em loop infinito (é quando ultrapassa o tamamnho máximo 272384)
                  // este ciclo infinito ocorre pelo menos no write...
#define SIZE 2500



int main() {

    char *path = "/f1";
    pthread_t tid[5];

    tfs_open_paramts input;
    tfs_open_paramts input1;
    tfs_open_paramts input2;
    tfs_open_paramts input3;
    tfs_open_paramts input4;

    input.pth = path;
    input.flg = TFS_O_CREAT;
    input1.pth = "/f2";
    input1.flg = TFS_O_CREAT;
    input2.pth = "/f3";
    input2.flg = TFS_O_CREAT;
    input3.pth = "/f4";
    input3.flg = TFS_O_CREAT;
    input4.pth = "/f5";
    input4.flg = TFS_O_CREAT;
    //int open_return_values[5];
    
    assert(tfs_init() != -1);
    //input.pth = "/f6";
    pthread_create(&tid[0], NULL, tfs_open, (void *)&input);
    
    pthread_create(&tid[1], NULL, tfs_open, (void *)&input1);
    
    pthread_create(&tid[2], NULL, tfs_open, (void *)&input2);

    pthread_create(&tid[3], NULL, tfs_open, (void *)&input3);
        
    pthread_create(&tid[4], NULL, tfs_open, (void *)&input4);

    /*for(int i = 1; i < 5; i++) {
        pthread_create(&tid[i], NULL, tfs_open2, (void *)&input);
        input.pth = "/f"+(i + '0');
    }*/
    /*if (pthread_create (&tid[3], NULL, fnBob, (void*)&m) != 0)
    exit(EXIT_FAILURE);*/
    
    /*
    for(int i = 0; i < 5; i++) {
        void **thread_return = NULL;
        pthread_join(tid[i], thread_return); //problema
        //int *return_int = ((int*) *thread_return);
        //open_return_values[i] = *return_int;
        //printf("tid: %d: %d\n", i, open_return_values[i]);
    }*/
    pthread_join(tid[0], NULL);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
    pthread_join(tid[4], NULL);

    print_dir(5);

    assert(tfs_destroy() != -1);
    /*
    for(int i; i < 5; i ++) {
        tfs_close(open_return_values[i]);
    }*/
    //assert(tfs_close(fd) != -1);

    return 0;
}