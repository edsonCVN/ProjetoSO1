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


0  1  2  3  4  5  6  7  8  9  10(indireto) (índices de i_data_block[])
2  3  5  7 -1 -1 -1 -1 -1 -1 -1


perguntar ao stor na 5a: como aceder/utilizar os valores de retorno do pthread_join


ATUALIZAR AS ASSINATURAS DO .h