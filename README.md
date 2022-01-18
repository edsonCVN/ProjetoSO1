 # ProjectoSO1
29/12/2021

    verificar funcionamento correto das diretorias

    funções a verficar thread-safety:
        -inode_create
        -find_in_dir
        -inode_get
        -add_dir_entry
        -inode_delete
        -add_to_open_file_table
        -remove_from_open_file_table
        -get_open_file_entry
        -data_block_alloc
        -data_block_get
        -touch_all_memory (->insert_delay)
        (FIQUEI NA LINHA 177 do state.c)

    ver condições de erro mal feitas no read

    PÔR DELETE DE DIRETORIAS A FUNCIONAR! (inode_delete) (perguntar ao stor se é código lixo)


obs: podiamos fazer trincos mais facies (i.e. mais globais) e certificarmo-nos que funcionavam. depois, começacamos a ser mais minuciosos e iamos alterando e testando de forma incremental


tfs_open Trucate
    1) não está preparado para truncar ficheiro que utilze somente 4 blocos direto (à 5ºa referencia na iteração ele vai injetar -1 (pq esta ref. não esstá a ser utiizada) no data_block_free() e levanta um erro desnecessário)
    2) não está preparado para aceder ao bloco indireto.


perguntar ao stor na 5a: 
    -como aceder/utilizar os valores de retorno do pthread_join;
    -pedir opinião sobre a granulidade dos nossos trincos;
    -copy_to_external: como aplicar trincos de forma a só dar lock de escrita quando a segunda thread tiver o mesmo dest_path e de forma a que uma segunda thread não consiga alterar o source_path;
    -necessidade/importância de pôr o tfs_destroy thread-safe
    

ATUALIZAR AS ASSINATURAS DO .h

deveriamos criar funcções para inicializar as estruturas de parametros.. (para evitar que o esqeucimento faça com que código lixo se propague)



resumo do horário de dúvidas:
    cumprir api no enunciado dentro do operations.c +  criar funções intermédias que utilizem as estruturas (DONE!)

    usar trincos para as estruturas partilhadas do state.c

    testar tfs open com + de 5 threads (DONE!)

    criar função que dá um novo trinco (mutex) a cada dest_path diferente
        a utlizar no início do "do"  da função copy_to_external

    tfs_open: usar trinco na verificação e operação (criar igual verificação para cada operação para aumentar paralelismo)

LDFLAGS += -lpthread
CFLAGS += -fsanitize=thread
LDFLAGS += -fsanitize=thread
CXXFLAGS = -fsanitize=thread


passar open ->write->close para dentro de uma função (para ser tudo chamado por uma thread)

colocar cadeados no offset, testar e aumentar o n. de testes
testar as várias flags do open

-> garantir que todos os locks têm destory

teste ideia: fazer read e write simultâneo com o mesmo filehandle.


PARA FAZER:
    corrigir no inode_delete (fatorizar o que está no truncate para usar lá)
    comentar os testes

    14/01/2022

    1 // confirmar a necessidade de fazer o join antes de chamar as leituras 1W2R
    4 // veririfcar maximo ficheiros abertos
    5 // submeter
    6 // strcmp

    test17 está a dar mal





    ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {

    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }
    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);

    pthread_rwlock_wrlock(&inode->i_rwlock);

    if (inode == NULL) {

        pthread_mutex_unlock(&file->of_mutex);
        pthread_rwlock_unlock(&inode->i_rwlock);

        return -1;
    }

    size_t size_to_write = to_write;
    size_t size_written = 0;

    while (size_to_write > size_written && inode->i_size < FILE_MAXSIZE) {

        if (file->of_offset / BLOCK_SIZE < 10) {
           
            /*
            size_t block_index = file->of_offset / BLOCK_SIZE;
            for (size_t j = block_index, block_offset = 0; j < INODE_BLOCKS_SIZE - 1 && size_written != size_to_write; j++) {

                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;

                if (to_write + block_offset > BLOCK_SIZE) {
                    to_write = BLOCK_SIZE - block_offset;
                }

                if (to_write > 0) {

                    if (inode->i_data_block[j] == -1) {
                        
                        inode->i_data_block[j] = data_block_alloc();
                    }

                    void *block = data_block_get(inode->i_data_block[j]);
                    if (block == NULL) {

                        pthread_rwlock_unlock(&inode->i_rwlock);
                        pthread_mutex_unlock(&file->of_mutex);

                        return -1;
                    }

                    
                    memcpy(block + block_offset, buffer + size_written, to_write);

                   

                    file->of_offset += to_write;
                    size_written += to_write;

                    if (file->of_offset > inode->i_size) {
                        inode->i_size = file->of_offset;
                    }
                }
            } */
        } else {

            if (inode->i_data_block[INDIRECT_BLOCK_INDEX] == -1) {
                /* Allocation of inode's indirect block. */
                inode->i_data_block[INDIRECT_BLOCK_INDEX] = data_block_alloc();

                int *indirect_block = data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
                if (indirect_block == NULL) {

                    pthread_rwlock_unlock(&inode->i_rwlock);
                    pthread_mutex_unlock(&file->of_mutex);

                    return -1;
                }

                /* Initialization of inode's indirect block's table. */
                for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
                    indirect_block[i] = -1;
                }
            }

            int *indirect_block = (int *)data_block_get(inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {

                pthread_rwlock_unlock(&inode->i_rwlock);
                pthread_mutex_unlock(&file->of_mutex);

                return -1;
            }

            size_t indirect_i = file->of_offset / BLOCK_SIZE - 10;
            for (size_t i = indirect_i, block_offset = 0; i < (BLOCK_SIZE / sizeof(int)) && size_to_write != size_written; i++) {

                block_offset = file->of_offset % BLOCK_SIZE;
                to_write = size_to_write - size_written;

                if (to_write + block_offset > BLOCK_SIZE) {
                    to_write = BLOCK_SIZE - block_offset;
                }
                if (to_write > 0) {
                    if (indirect_block[i] == -1) {
                        indirect_block[i] = data_block_alloc();
                    }

                    void *block = data_block_get(indirect_block[i]);
                    if (block == NULL) {

                        pthread_rwlock_unlock(&inode->i_rwlock);
                        pthread_mutex_unlock(&file->of_mutex);

                        return -1;
                    }
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
        }
    }

    pthread_rwlock_unlock(&inode->i_rwlock);
    pthread_mutex_unlock(&file->of_mutex);

    return (ssize_t)size_written;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {

    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }
    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);

    pthread_rwlock_rdlock(&inode->i_rwlock);

    if (inode == NULL) {

        pthread_mutex_unlock(&file->of_mutex);
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

        if (initial_offset / BLOCK_SIZE < 10) {
            for (size_t j = initial_offset / BLOCK_SIZE; to_read > 0 && j < INODE_BLOCKS_SIZE - 1; j++) {

                char *block = (char *)data_block_get(inode->i_data_block[j]);

                if (block == NULL) {

                    pthread_mutex_unlock(&file->of_mutex);
                    pthread_rwlock_unlock(&inode->i_rwlock);

                    return -1;
                }

                /* Perform the actual read */
                if (j == initial_offset / BLOCK_SIZE) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read = BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset, amount_of_bytes_to_read);
                } else {
                    amount_of_bytes_to_read = BLOCK_SIZE;
                    if (to_read < BLOCK_SIZE - (file->of_offset % BLOCK_SIZE)) {
                        amount_of_bytes_to_read = to_read;
                    }

                    memcpy(buffer + read, block, amount_of_bytes_to_read);
                }

                /* The offset associated with the file handle is
                 * incremented accordingly */
                file->of_offset += amount_of_bytes_to_read;

                to_read -= amount_of_bytes_to_read;
                read += amount_of_bytes_to_read;
            }

        } else {
            int *indirect_block = (int *)data_block_get(
                inode->i_data_block[INDIRECT_BLOCK_INDEX]);
            if (indirect_block == NULL) {

                pthread_mutex_unlock(&file->of_mutex);
                pthread_rwlock_unlock(&inode->i_rwlock);

                return -1;
            }

            size_t first_indirect_i = file->of_offset / BLOCK_SIZE - 10;
            for (size_t i = first_indirect_i; i < (BLOCK_SIZE / sizeof(int)) && to_read > 0; i++) {

                char *block = (char *)data_block_get(indirect_block[i]);

                if (block == NULL) {

                    pthread_mutex_unlock(&file->of_mutex);
                    pthread_rwlock_unlock(&inode->i_rwlock);

                    return -1;
                }

                /* Perform the actual read */
                if (i == first_indirect_i) {
                    if (to_read > BLOCK_SIZE - initial_block_offset) {
                        amount_of_bytes_to_read = BLOCK_SIZE - initial_block_offset;
                    } else {
                        amount_of_bytes_to_read = to_read;
                    }
                    memcpy(buffer + read, block + initial_block_offset, amount_of_bytes_to_read);
                } else {
                    amount_of_bytes_to_read = BLOCK_SIZE;
                    if (to_read < BLOCK_SIZE - (file->of_offset % BLOCK_SIZE)) {
                        amount_of_bytes_to_read = to_read;
                    }
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

    pthread_mutex_unlock(&file->of_mutex);
    pthread_rwlock_unlock(&inode->i_rwlock);

    return (ssize_t)read;
}