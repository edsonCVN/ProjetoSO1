while(size_to_write > size_written) {
    switch (inode->i_size < (INODE_BLOCKS_SIZE-1) * (BLOCK_SIZE))}
    {
        case 1: // (true) do 1 ... 10 blocos
            for que já temos
        break; 

        case 0: // (false) do 12 ... n-necessario blocos
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
        for(size_t i = indirect_i; size_to_write != size_written; i++) { //a condição size_to_write != size_written deve tornar o while escusado...
            
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
                
                memcpy(block + file->of_offset, buffer + size_written, to_write);
                
                file->of_offset += to_write;
                size_written += to_write;

                if (file->of_offset > inode->i_size) {
                    inode->i_size = file->of_offset; 
                }
            }
        }
        break;

        default:
        break;
    }
}

