#include "operations.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tfs_init() {
    state_init();

    /* create root inode */
    int root = inode_create(T_DIRECTORY);
    if (root != ROOT_DIR_INUM) {
        return -1;
    }

    return 0;
}

int tfs_destroy() {
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

int tfs_open(char const *name, int flags) {
    int inum;
    size_t offset;

    /* Checks if the path name is valid */
    if (!valid_pathname(name)) {
        return -1;
    }

    inum = tfs_lookup(name);
    if (inum >= 0) {
        /* The file already exists */
        inode_t *inode = inode_get(inum);
        if (inode == NULL) {
            return -1;
        }

        /* Trucate (if requested) */
        if (flags & TFS_O_TRUNC) { //FIXME mudar para quando for indireto
            if (inode->i_size > 0) {
                for(int j = 0; j < INODE_BLOCKS_SIZE; j++) { 
                    if (data_block_free(inode->i_data_block[j]) == -1) {
                        return -1;
                    }
                }
                inode->i_size = 0;
            }
        }
        /* Determine initial offset */
        if (flags & TFS_O_APPEND) {
            offset = inode->i_size;
        } else {
            offset = 0;
        }
    } else if (flags & TFS_O_CREAT) {
        /* The file doesn't exist; the flags specify that it should be created*/
        /* Create inode */
        inum = inode_create(T_FILE);
        if (inum == -1) {
            return -1;
        }
        /* Add entry in the root directory */
        if (add_dir_entry(ROOT_DIR_INUM, inum, name + 1) == -1) {
            inode_delete(inum);
            return -1;
        }
        offset = 0;
    } else {
        return -1;
    }

    /* Finally, add entry to the open file table and
     * return the corresponding handle */
    return add_to_open_file_table(inum, offset);

    /* Note: for simplification, if file was created with TFS_O_CREAT and there
     * is an error adding an entry to the open file table, the file is not
     * opened but it remains created */
}


int tfs_close(int fhandle) { return remove_from_open_file_table(fhandle); }

ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);
    if (inode == NULL) {
        return -1;
    }

    
    size_t size_to_write = to_write;
    size_t size_written = 0;
    
    while(size_to_write > size_written) {
        
        if(inode->i_size < (INODE_BLOCKS_SIZE-1) * (BLOCK_SIZE)) {
            size_t block_index = file->of_offset / BLOCK_SIZE;
            for(size_t j = block_index, block_offset = 0; j < INODE_BLOCKS_SIZE - 1 && size_written != size_to_write; j++) { 
                
                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;

                if (to_write + block_offset > BLOCK_SIZE) {
                    to_write = BLOCK_SIZE- block_offset;
                }

                if (to_write > 0) {
                    if (inode->i_data_block[j] == -1) {
                        /* If empty file, allocate new block */
                        inode->i_data_block[j] = data_block_alloc();
                    }

                    void *block = data_block_get(inode->i_data_block[j]);
                    if (block == NULL) {
                        return -1;
                    }

                    /* Perform the actual write */
                    memcpy(block + block_offset, buffer + size_written, to_write);
                    
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
                inode->i_data_block[INDIRECT_BLOCK_INDEX] = data_block_alloc(); //alocação do bloco indireto
                
                //verificar se pode ser do tipo int*!
                int *indirect_block = data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
                if (indirect_block == NULL) {
                    return -1;
                }
                //inicialização das referencias dentro do bloco indireto (só funciona se o cast anterior para int funcionar!)
                for(int i = 0; i < BLOCK_SIZE/sizeof(int); i++) {
                    indirect_block[i] = -1;
                }
            }
            //ver forma de não repetir o block_get (apesar que no if acima é caso ainda não tenha cido alocado)
            int *indirect_block = data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {
                return -1;
            }

            size_t indirect_i = (file->of_offset - BLOCK_SIZE * 10) / BLOCK_SIZE; //para não contar com o 11th bloco
            for(size_t i = indirect_i, block_offset = 0; size_to_write != size_written; i++) { //a condição size_to_write != size_written deve tornar o while escusado...
                
                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;
                
                if (to_write + block_offset > BLOCK_SIZE) {
                to_write = BLOCK_SIZE - block_offset; //(i-1) pq o indirect_i tem o deslocamento para não aceder ao 11th bloco
                }
                if (to_write > 0) {
                    if (indirect_block[i] == -1) {
                        /* If empty file, allocate new block */
                        indirect_block[i] = data_block_alloc();
                    }

                    void *block = data_block_get(indirect_block[i]);
                    if (block == NULL) {
                        return -1;
                    }
                    
                    memcpy(block + block_offset, buffer + size_written, to_write);
                    
                    file->of_offset += to_write;
                    size_written += to_write;

                    if (file->of_offset > inode->i_size) {
                        inode->i_size = file->of_offset; 
                    }
                }
            }
        }
    
    }
    return (ssize_t)size_written;
}


ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);
    if (inode == NULL) {
        return -1;
    }

    /* Determine how many bytes to read */
    size_t to_read = inode->i_size - file->of_offset;
    if (to_read > len) {
        to_read = len;
    }

    size_t read = 0;
    while(to_read > 0){
        size_t initial_offset = file->of_offset;
        size_t amount_of_bytes_to_read;
        size_t initial_block_offset = file->of_offset % BLOCK_SIZE;
        if(initial_offset < (INODE_BLOCKS_SIZE-1) * (BLOCK_SIZE)) {
            for(size_t j = initial_offset / BLOCK_SIZE; to_read > 0 && j < INODE_BLOCKS_SIZE - 1; j++) {
                        
                char *block = (char*) data_block_get(inode->i_data_block[j]);
                        
                if (j == (initial_offset / BLOCK_SIZE) && block == NULL) { 
                    return -1;
                }
                if(block == NULL) {//merge coma  de cima e retornar -1
                    break;
                }

                /* Perform the actual read */
                if(j == initial_offset / BLOCK_SIZE) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read = BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset, amount_of_bytes_to_read);
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
            int *indirect_block = data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {
                return -1;
            }

            size_t first_indirect_i = (file->of_offset - BLOCK_SIZE * 10) / BLOCK_SIZE; //para não contar com o 11th bloco
            for(size_t i = first_indirect_i; to_read != read; i++) { //a condição size_to_write != size_written deve tornar o while escusado...
                    
                char *block = (char*) data_block_get(indirect_block[i]);
                        
                if (i == first_indirect_i && block == NULL) { 
                    return -1;
                }
                if(block == NULL) {//merge coma  de cima e retornar -1
                    break;
                }

                /* Perform the actual read */
                if(i == first_indirect_i) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read = BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset, amount_of_bytes_to_read);
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
    return (ssize_t)read;
}
