#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#define COUNT 109 // a partir de = 109 ele fica em loop infinito (é quando ultrapassa o tamamnho máximo 272384)
                  // este ciclo infinito ocorre pelo menos no write...
#define SIZE 2500

/**
   This test fills in a new file up to 20 blocks via multiple writes
   (therefore causing the file to hold 10 direct references + 10 indirect
   references from a reference block),
   each write always targeting only 1 block of the file, 
   then checks if the file contents are as expected
 */

int main() {

    char *path = "/f1";
    pthread_t tid[5];

    tfs_open_input input;
    input.pth = path;
    input.flg = TFS_O_CREAT;
    //int open_return_values[5];
    
    assert(tfs_init() != -1);
    input.pth = "/f1";
    pthread_create(&tid[0], NULL, tfs_open2, (void *)&input);
    
    input.pth = "/f2";
    pthread_create(&tid[1], NULL, tfs_open2, (void *)&input);
    
    input.pth = "/f3";
    pthread_create(&tid[2], NULL, tfs_open2, (void *)&input);

    input.pth = "/f4";
    pthread_create(&tid[3], NULL, tfs_open2, (void *)&input);
    input.pth = "/f5";
    pthread_create(&tid[4], NULL, tfs_open2, (void *)&input);

    /*for(int i = 1; i < 5; i++) {
        pthread_create(&tid[i], NULL, tfs_open2, (void *)&input);
        input.pth = "/f"+(i + '0');
    }*/
    /*if (pthread_create (&tid[3], NULL, fnBob, (void*)&m) != 0)
    exit(EXIT_FAILURE);*/
    
    for(int i = 0; i < 5; i++) {
        void **thread_return = NULL;
        pthread_join(tid[i], thread_return); //problema
        //int *return_int = ((int*) *thread_return);
        //open_return_values[i] = *return_int;
        //printf("tid: %d: %d\n", i, open_return_values[i]);
    }

    print_dir(5);

    assert(tfs_destroy() != -1);
    /*
    for(int i; i < 5; i ++) {
        tfs_close(open_return_values[i]);
    }*/
    //assert(tfs_close(fd) != -1);

    return 0;
}