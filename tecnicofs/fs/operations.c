#include "operations.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

pthread_mutex_t global_lock;

int tfs_init() { // supondo que tfs_init() nunca vai ser chamado de forma
                 // concorrente, não há problema
    state_init();
    // pthread_mutex_init(&global_lock, NULL);

    /* create root inode */
    int root = inode_create(T_DIRECTORY);
    if (root != ROOT_DIR_INUM) {
        return -1;
    }

    inode_t *root_inode = inode_get(ROOT_DIR_INUM);
    pthread_rwlock_init(&root_inode->i_rwlock, NULL);

    return 0;
}

int tfs_destroy() { // ver que estruturas a destruir (inodes, dir_entires,
                    // rwlocks...)
    // pthread_mutex_destroy(&global_lock);
    inode_t *root_inode = inode_get(ROOT_DIR_INUM);
    pthread_rwlock_destroy(&root_inode->i_rwlock);

    state_destroy();
    return 0;
}

static bool valid_pathname(char const *name) {
    return name != NULL && strlen(name) > 1 && name[0] == '/';
}

int tfs_lookup(char const *name) {
    if (!valid_pathname(name)) {
        return -1;
    }

    // skip the initial '/' character
    name++;

    return find_in_dir(ROOT_DIR_INUM, name);
}

// função teste
void print_dir(int n_entries) { print_dir_state(n_entries, ROOT_DIR_INUM); }
// função teste

int tfs_open(char const *name, int flags){
    
    int inum;
    int rtn_value;
    size_t offset;
    inode_t *inode;

    /* Checks if the path name is valid */
    if (!valid_pathname(name)) {
        printf("INVALID PTH\n");
        return -1;
    }

    inum = tfs_lookup(name); //A ASSUMIR QUE tfs_lookup é thread-safe
    if (inum >= 0) { //testar este if (thread-safety)
        /* The file already exists */
        inode = inode_get(inum);
        if (inode == NULL) {
            return -1;
        }
        pthread_rwlock_wrlock(&inode->i_rwlock);
        /* Trucate (if requested) */
        if (flags & TFS_O_TRUNC) { //dar fix ao for e testar thread-safety
            if (inode->i_size > 0) {
                size_t to_delete = inode->i_size;
                for (int j = 0; j < INODE_BLOCKS_SIZE && to_delete >= BLOCK_SIZE; j++) {
                    if (data_block_free(inode->i_data_block[j]) == -1) {
                        pthread_rwlock_unlock(&inode->i_rwlock);
                        return -1;
                    }
                    to_delete -= BLOCK_SIZE;
                    if(j == INDIRECT_BLOCK_INDEX) {
                        int *indirect_block = data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
                        if (indirect_block == NULL) {
                            break; //ainda não foi utilizado
                        }
                        for(int k = 0; k < BLOCK_SIZE / sizeof(int) && to_delete >= BLOCK_SIZE; k++){
                            if (data_block_free(inode->i_data_block[k]) == -1) {
                                pthread_rwlock_unlock(&inode->i_rwlock);
                                return -1;
                            }
                            to_delete -= BLOCK_SIZE;
                        }
                    }
                }
                inode->i_size = 0;
                pthread_rwlock_unlock(&inode->i_rwlock);
            }
        }
        /* Determine initial offset */
        if (flags & TFS_O_APPEND) {
            offset = inode->i_size;
        } else {
            offset = 0;
        }
    } else if (flags & TFS_O_CREAT) {
        inode = inode_get(ROOT_DIR_INUM);
        pthread_rwlock_wrlock(&inode->i_rwlock);
        /* The file doesn't exist; the flags specify that it should be created*/
        /* Create inode */
        inum = inode_create(T_FILE);
        if (inum == -1) {
            pthread_rwlock_unlock(&inode->i_rwlock);
            return -1;
        }
        /* Add entry in the root directory */
        if (add_dir_entry(ROOT_DIR_INUM, inum, name + 1) == -1) {
            inode_delete(inum);
            pthread_rwlock_unlock(&inode->i_rwlock);
            return -1;
        }
        offset = 0;
    } else {
        return -1;
    }
    // SECÇÃO CRÍTICA fim (antes do return porque se pode abrir o ficheiro duas
    // vezes) 

    rtn_value = add_to_open_file_table(inum, offset); //assumindo thread-safe...
    //pthread_rwlock_unlock(&root_inode->i_rwlock);
    /* Finally, add entry to the open file table and
     * return the corresponding handle */
    pthread_rwlock_unlock(&inode->i_rwlock);
    return rtn_value;

    /* Note: for simplification, if file was created with TFS_O_CREAT and there
     * is an error adding an entry to the open file table, the file is not
     * opened but it remains created */
}

int tfs_close(int fhandle) { return remove_from_open_file_table(fhandle); }

ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {

    // SECÇÃO CRITICA ESCRITA
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);

    pthread_rwlock_wrlock(&inode->i_rwlock);

    if (inode == NULL) {
        pthread_rwlock_unlock(&inode->i_rwlock);
        return -1;
    }

    size_t size_to_write = to_write;
    size_t size_written = 0;
    // dúvida: verificações necessariamente dentro do mesmo cadeado que as
    // operações? GARANTIR QUE ALTERAÇÃO DO *inode->i_size* e *file->of_offset*
    // é thread-safe!

    while (size_to_write > size_written && inode->i_size < FILE_MAXSIZE) {

        if (inode->i_size < (INODE_BLOCKS_SIZE - 1) * (BLOCK_SIZE)) {
            size_t block_index = file->of_offset / BLOCK_SIZE;
            for (size_t j = block_index, block_offset = 0;
                 j < INODE_BLOCKS_SIZE - 1 && size_written != size_to_write;
                 j++) {

                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;

                if (to_write + block_offset > BLOCK_SIZE) {
                    to_write = BLOCK_SIZE - block_offset;
                }

                if (to_write > 0) {

                    if (inode->i_data_block[j] == -1) {
                        /* If empty file, allocate new block */
                        inode->i_data_block[j] = data_block_alloc();
                    }

                    void *block = data_block_get(inode->i_data_block[j]);
                    if (block == NULL) {
                        pthread_rwlock_unlock(&inode->i_rwlock);
                        return -1;
                    }

                    /* Perform the actual write */
                    memcpy(block + block_offset, buffer + size_written,
                           to_write);

                    /* The offset associated with the file handle is
                     * incremented accordingly */

                    file->of_offset += to_write;
                    size_written += to_write;

                    if (file->of_offset > inode->i_size) {
                        inode->i_size = file->of_offset;
                    }
                }
            }
        } else {

            if (inode->i_data_block[INDIRECT_BLOCK_INDEX] == -1) {
                inode->i_data_block[INDIRECT_BLOCK_INDEX] =
                    data_block_alloc(); // alocação do bloco indireto

                int *indirect_block =
                    data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
                if (indirect_block == NULL) {
                    pthread_rwlock_unlock(&inode->i_rwlock);
                    return -1;
                }
                // inicialização das referencias dentro do bloco indireto

                for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
                    indirect_block[i] = -1;
                }
            }
            // ver forma de não repetir o block_get (apesar que no if acima é
            // caso ainda não tenha cido alocado)
            int *indirect_block =
                data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {
                pthread_rwlock_unlock(&inode->i_rwlock);
                return -1;
            }

            size_t indirect_i = (file->of_offset - BLOCK_SIZE * 10) /
                                BLOCK_SIZE; // para não contar com o 11th bloco
            for (size_t i = indirect_i, block_offset = 0;
                 i < (BLOCK_SIZE / sizeof(int)) &&
                 size_to_write != size_written;
                 i++) {

                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;

                if (to_write + block_offset > BLOCK_SIZE) {
                    to_write =
                        BLOCK_SIZE -
                        block_offset; //(i-1) pq o indirect_i tem o deslocamento
                                      // para não aceder ao 11th bloco
                }
                if (to_write > 0) {
                    if (indirect_block[i] == -1) {
                        /* If empty file, allocate new block */
                        indirect_block[i] = data_block_alloc();
                    }

                    void *block = data_block_get(indirect_block[i]);
                    if (block == NULL) {
                        pthread_rwlock_unlock(&inode->i_rwlock);
                        return -1;
                    }
                    memcpy(block + block_offset, buffer + size_written,
                           to_write);

                    file->of_offset += to_write;
                    size_written += to_write;

                    if (file->of_offset > inode->i_size) {
                        inode->i_size = file->of_offset;
                    }
                }
            }
        }
    }
    // SECÇÃO CRITICA ESCRITA
    pthread_rwlock_unlock(&inode->i_rwlock);
    return (ssize_t)size_written;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    
    // SECÇÃO CRITICA ESCRITA
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);
    
    pthread_rwlock_rdlock(&inode->i_rwlock);
    
    if (inode == NULL) {
        pthread_rwlock_unlock(&inode->i_rwlock);
        return -1;
    }

    /* Determine how many bytes to read */
    size_t to_read = inode->i_size - file->of_offset;
    if (to_read > len) {
        to_read = len;
    }

    size_t read = 0;
    while (to_read > 0) {
        size_t initial_offset = file->of_offset;
        size_t amount_of_bytes_to_read;
        size_t initial_block_offset = file->of_offset % BLOCK_SIZE;
        if (initial_offset < (INODE_BLOCKS_SIZE - 1) * (BLOCK_SIZE)) {
            for (size_t j = initial_offset / BLOCK_SIZE;
                 to_read > 0 && j < INODE_BLOCKS_SIZE - 1; j++) {

                char *block = (char *)data_block_get(inode->i_data_block[j]);

                if (block == NULL) {
                    pthread_rwlock_unlock(&inode->i_rwlock);
                    return -1;
                }

                /* Perform the actual read */
                if (j == initial_offset / BLOCK_SIZE) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read =
                            BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset,
                           amount_of_bytes_to_read);
                } else {
                    amount_of_bytes_to_read = to_read;
                    memcpy(buffer + read, block, amount_of_bytes_to_read);
                }

                /* The offset associated with the file handle is
                 * incremented accordingly */
                file->of_offset += amount_of_bytes_to_read;
                to_read -= amount_of_bytes_to_read;
                read += amount_of_bytes_to_read;
            }

        } else {
            int *indirect_block =
                data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {
                pthread_rwlock_unlock(&inode->i_rwlock);
                return -1;
            }

            size_t first_indirect_i =
                (file->of_offset - BLOCK_SIZE * 10) /
                BLOCK_SIZE; // para não contar com o 11th bloco
            for (size_t i = first_indirect_i;
                 i < (BLOCK_SIZE / sizeof(int)) && to_read > 0;
                 i++) {

                char *block = (char *)data_block_get(indirect_block[i]);

                if (block == NULL) {
                    pthread_rwlock_unlock(&inode->i_rwlock);
                    return -1;
                }

                /* Perform the actual read */
                if (i == first_indirect_i) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read =
                            BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset,
                           amount_of_bytes_to_read);
                } else {
                    amount_of_bytes_to_read = to_read;
                    memcpy(buffer + read, block, amount_of_bytes_to_read);
                }

                /* The offset associated with the file handle is
                 * incremented accordingly */

                file->of_offset += amount_of_bytes_to_read;
                to_read -= amount_of_bytes_to_read;
                read += amount_of_bytes_to_read;
            }
        }
    }
    // SECÇÃO CRITICA ESCRITA
    pthread_rwlock_unlock(&inode->i_rwlock);
    return (ssize_t)read;
}

int tfs_copy_to_external_fs(char const *src_path, char const *dest_path) {
    
    char buffer[BLOCK_SIZE];
    ssize_t bytes_read = 0;
    int error = 0;
    // SECÇÃO CRITICA ESCRITA

    int src_file = tfs_open(src_path, 0);

    if (src_file == -1) {
        return -1;
    }

    FILE *dest_fp = fopen(dest_path, "w");
    if (dest_fp == NULL) {
        tfs_close(src_file);
        return -1;
    }
   
    //trinco global
    do {
        bytes_read = tfs_read(src_file, buffer, BLOCK_SIZE);
        if (bytes_read == -1 ||
            bytes_read !=
                fwrite(buffer, sizeof(char), (size_t)bytes_read, dest_fp)) {
            error += -1;
        }
    } while (bytes_read == BLOCK_SIZE && error == 0);

    error += tfs_close(src_file);
    error += fclose(dest_fp);
    // SECÇÃO CRITICA ESCRITA
    if (error != 0) {
        return -1;
    }
    return 0;
}
