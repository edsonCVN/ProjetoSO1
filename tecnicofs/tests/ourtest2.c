#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <threads_api.h>
#include <pthread.h>
#define COUNT 109 // a partir de = 109 ele fica em loop infinito (é quando ultrapassa o tamamnho máximo 272384)
                  // este ciclo infinito ocorre pelo menos no write...
#define SIZE 2500



int main() {

    char *path = "/f1";
    pthread_t tid[9];

    tfs_open_paramts input[9];

    input[0].pth = path;
    input[0].flg = TFS_O_CREAT;
    input[1].pth = "/f2";
    input[1].flg = TFS_O_TRUNC;
    input[2].pth = "/f2";
    input[2].flg = TFS_O_TRUNC;
    input[3].pth = "/f2";
    input[3].flg = TFS_O_TRUNC;
    input[4].pth = "/f2";
    input[4].flg = TFS_O_TRUNC;
    input[5].pth = "/f2";
    input[5].flg = TFS_O_TRUNC;
    input[6].pth = "/f2";
    input[6].flg = TFS_O_TRUNC;
    input[7].pth = "/f2";
    input[7].flg = TFS_O_TRUNC;
    input[8].pth = "/f2";
    input[8].flg = TFS_O_TRUNC;

  
    
    assert(tfs_init() != -1);
    
    for (int i =0; i <9; i++) {
        pthread_create(&tid[i], NULL, tfs_open_tapi, (void *)&input[i]);
    }
    for (int i =0; i <9; i++) {
        pthread_join(tid[i], NULL);    
    }
    
    print_dir(9);

    assert(tfs_destroy() != -1);
  

    return 0;
}